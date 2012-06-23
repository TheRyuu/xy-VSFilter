/************************************************************************/
/* author: xy                                                           */
/* date: 20110926                                                       */
/************************************************************************/
#include "StdAfx.h"
#include "cache_manager.h"
#include "draw_item.h"
#include "xy_overlay_paint_machine.h"
#include "xy_clipper_paint_machine.h"

ULONG PathDataTraits::Hash( const PathData& key )
{
    ULONG hash = 515;
    hash += (hash<<5);
    hash += key.mPathPoints;
    for (int i=0;i<key.mPathPoints;i++)
    {
        hash += (hash<<5);
        hash += key.mpPathTypes[i];
    }
    for (int i=0;i<key.mPathPoints;i++)
    {
        hash += (hash<<5);
        hash += key.mpPathPoints[i].x;
        hash += (hash<<5);
        hash += key.mpPathPoints[i].y;
    }
    return hash;
}

ULONG ClipperTraits::Hash( const CClipper& key )
{
    ULONG hash = CStringElementTraits<CString>::Hash(key.m_polygon->m_str);;
    hash += (hash<<5);
    hash += key.m_inverse;
    hash += (hash<<5);
    hash += key.m_effectType;
    hash += (hash<<5);
    hash += key.m_size.cx;
    hash += (hash<<5);
    hash += key.m_size.cy;
    hash += (hash<<5);
    hash += hash_value(key.m_polygon->m_scalex);
    hash += (hash<<5);
    hash += hash_value(key.m_polygon->m_scaley);

    for (int i=0;i<sizeof(key.m_effect.param)/sizeof(key.m_effect.param[0]);i++)
    {
        hash += (hash<<5);
        hash += key.m_effect.param[i];
    }
    for (int i=0;i<sizeof(key.m_effect.t)/sizeof(key.m_effect.t[0]);i++)
    {
        hash += (hash<<5);
        hash += key.m_effect.t[i];
    }
    return hash;
}

//////////////////////////////////////////////////////////////////////////////////////////////

// TextInfoCacheKey

bool TextInfoCacheKey::operator==( const TextInfoCacheKey& key ) const
{
    return m_str == key.m_str 
        && static_cast<const STSStyleBase&>(m_style).operator==(key.m_style)
        && m_style.get().fontScaleX == key.m_style.get().fontScaleX
        && m_style.get().fontScaleY == key.m_style.get().fontScaleY
        && m_style.get().fontSpacing == key.m_style.get().fontSpacing;
}

ULONG TextInfoCacheKey::UpdateHashValue() 
{
    m_hash_value = CStringElementTraits<CString>::Hash(m_str);
    m_hash_value += (m_hash_value<<5);
    m_hash_value += hash_value( static_cast<const STSStyleBase&>(m_style.get()) );
    m_hash_value += (m_hash_value<<5);
    m_hash_value += hash_value( m_style.get().fontScaleX );
    m_hash_value += (m_hash_value<<5);
    m_hash_value += hash_value( m_style.get().fontScaleY );
    m_hash_value += (m_hash_value<<5);
    m_hash_value += hash_value( m_style.get().fontSpacing );
    return m_hash_value;
}

//////////////////////////////////////////////////////////////////////////////////////////////

// CWordCacheKey

CWordCacheKey::CWordCacheKey( const CWord& word )
{
    m_str = word.m_str;
    m_style = word.m_style;
    m_ktype = word.m_ktype;
    m_kstart = word.m_kstart;
    m_kend = word.m_kend;
}

CWordCacheKey::CWordCacheKey( const CWordCacheKey& key )
{
    m_str = key.m_str;
    m_style = key.m_style;
    m_ktype = key.m_ktype;
    m_kstart = key.m_kstart;
    m_kend = key.m_kend;
    m_hash_value = key.m_hash_value;
}

CWordCacheKey::CWordCacheKey( const FwSTSStyle& style, const CStringW& str, int ktype, int kstart, int kend )
    :m_style(style),m_str(str),m_ktype(ktype),m_kstart(kstart),m_kend(m_kend)
{

}

bool CWordCacheKey::operator==( const CWordCacheKey& key ) const
{
    return (m_str == key.m_str &&
        m_style == key.m_style &&
        m_ktype == key.m_ktype &&
        m_kstart == key.m_kstart &&
        m_kend == key.m_kend);
}

bool CWordCacheKey::operator==(const CWord& key)const
{
    return (m_str == key.m_str &&
        m_style == key.m_style &&
        m_ktype == key.m_ktype &&
        m_kstart == key.m_kstart &&
        m_kend == key.m_kend);
}

ULONG CWordCacheKey::UpdateHashValue() 
{
    m_hash_value = CStringElementTraits<CString>::Hash(m_str);//fix me
    return m_hash_value;
}
//////////////////////////////////////////////////////////////////////////////////////////////

// PathDataCacheKey

bool PathDataCacheKey::CompareSTSStyle( const STSStyle& lhs, const STSStyle& rhs )
{
    return lhs.charSet==rhs.charSet &&
        lhs.fontName==rhs.fontName &&
        lhs.fontSize==rhs.fontSize &&
        lhs.fontSpacing==rhs.fontSpacing &&
        lhs.fontWeight==rhs.fontWeight &&
        lhs.fItalic==rhs.fItalic &&
        lhs.fUnderline==rhs.fUnderline &&
        lhs.fStrikeOut==rhs.fStrikeOut;
}

ULONG PathDataCacheKey::UpdateHashValue()
{
    m_hash_value = CStringElementTraits<CString>::Hash(m_str);
    return m_hash_value;
}


//////////////////////////////////////////////////////////////////////////////////////////////

// ScanLineData2CacheKey

bool ScanLineData2CacheKey::operator==( const ScanLineData2CacheKey& key ) const
{ 
    return //(static_cast<PathDataCacheKey>(*this)==static_cast<PathDataCacheKey>(key)) 
        PathDataCacheKey::operator==(key) //static_cast will call copy constructer to construct a tmp obj
        && this->m_style.get().borderStyle == key.m_style.get().borderStyle
        && fabs(this->m_style.get().outlineWidthX - key.m_style.get().outlineWidthX) < 0.000001
        && fabs(this->m_style.get().outlineWidthY - key.m_style.get().outlineWidthY) < 0.000001
        && fabs(this->m_style.get().fontScaleX - key.m_style.get().fontScaleX) < 0.000001
        && fabs(this->m_style.get().fontScaleY - key.m_style.get().fontScaleY) < 0.000001
        && fabs(this->m_style.get().fontAngleX - key.m_style.get().fontAngleX) < 0.000001
        && fabs(this->m_style.get().fontAngleY - key.m_style.get().fontAngleY) < 0.000001
        && fabs(this->m_style.get().fontAngleZ - key.m_style.get().fontAngleZ) < 0.000001
        && fabs(this->m_style.get().fontShiftX - key.m_style.get().fontShiftX) < 0.000001
        && fabs(this->m_style.get().fontShiftY - key.m_style.get().fontShiftY) < 0.000001
        && (m_org.x==key.m_org.x) && (m_org.y==key.m_org.y); 
}

ULONG ScanLineData2CacheKey::UpdateHashValue()
{
    m_hash_value = __super::UpdateHashValue();
    m_hash_value += (m_hash_value<<5);
    m_hash_value += m_org.x;
    m_hash_value += (m_hash_value<<5);
    m_hash_value += m_org.y;
    return  m_hash_value;
}

//////////////////////////////////////////////////////////////////////////////////////////////

// OverlayNoBlurKey

bool OverlayNoBlurKey::operator==( const OverlayNoBlurKey& key ) const
{ 
    //static_cast will call copy constructer to construct a tmp obj
    //return (static_cast<ScanLineDataCacheKey>(*this)==static_cast<ScanLineDataCacheKey>(key)) 
    //    && (m_p.x==key.m_p.x) && (m_p.y==key.m_p.y); 
    return ScanLineData2CacheKey::operator==(key) && (m_p.x==key.m_p.x) && (m_p.y==key.m_p.y);
}

ULONG OverlayNoBlurKey::UpdateHashValue()
{
    m_hash_value = __super::UpdateHashValue();
    m_hash_value += (m_hash_value<<5);
    m_hash_value += m_p.x;
    m_hash_value += (m_hash_value<<5);
    m_hash_value += m_p.y;
    return  m_hash_value;
}


//////////////////////////////////////////////////////////////////////////////////////////////

// OverlayKey

bool OverlayKey::operator==( const OverlayKey& key ) const
{    
    return OverlayNoBlurKey::operator==(key)
        && fabs(this->m_style.get().fGaussianBlur - key.m_style.get().fGaussianBlur) < 0.000001
        && this->m_style.get().fBlur == key.m_style.get().fBlur;
    //static_cast will call copy constructer to construct a tmp obj
    //return ((CWordCacheKey)(*this)==(CWordCacheKey)key) && (m_p.x==key.m_p.x) && (m_p.y==key.m_p.y) 
    //        && (m_org.x==key.m_org.x) && (m_org.y==key.m_org.y);
}

ULONG OverlayKey::UpdateHashValue()
{
    m_hash_value = __super::UpdateHashValue();
    m_hash_value += (m_hash_value<<5);
    m_hash_value += m_style.get().fBlur;
    m_hash_value += (m_hash_value<<5);
    m_hash_value += hash_value(m_style.get().fGaussianBlur);
    return  m_hash_value;
}


//////////////////////////////////////////////////////////////////////////////////////////////

// ScanLineDataCacheKey

bool ScanLineDataCacheKey::operator==( const ScanLineDataCacheKey& key ) const
{
    return (m_path_data && key.m_path_data) ? *m_path_data==*key.m_path_data : m_path_data==key.m_path_data;
}

ULONG ScanLineDataCacheKey::UpdateHashValue()
{
    m_hash_value = PathDataTraits::Hash(*m_path_data);
    return m_hash_value;
}

//////////////////////////////////////////////////////////////////////////////////////////////

// OverlayNoOffsetKey

bool OverlayNoOffsetKey::operator==( const OverlayNoOffsetKey& key ) const
{
    return (this==&key) || ( this->m_border == key.m_border && this->m_rasterize_sub == key.m_rasterize_sub &&
        ScanLineDataCacheKey::operator==(key) );
}

ULONG OverlayNoOffsetKey::UpdateHashValue()
{
    m_hash_value = __super::UpdateHashValue();
    m_hash_value += (m_hash_value<<5);
    m_hash_value += m_border;
    m_hash_value += (m_hash_value<<5);
    m_hash_value += m_rasterize_sub;
    return m_hash_value;
}


//////////////////////////////////////////////////////////////////////////////////////////////

// ClipperAlphaMaskCacheKey

bool ClipperAlphaMaskCacheKey::operator==( const ClipperAlphaMaskCacheKey& key ) const
{
    bool result = false;
    if (m_clipper==key.m_clipper)
    {
        result = true;
    }
    else if ( m_clipper!=NULL && key.m_clipper!=NULL )
    {
        const CClipper& lhs = *m_clipper;
        const CClipper& rhs = *key.m_clipper;
        result = (lhs.m_polygon->m_str == rhs.m_polygon->m_str 
            && fabs(lhs.m_polygon->m_scalex - rhs.m_polygon->m_scalex) < 0.000001
            && fabs(lhs.m_polygon->m_scaley - rhs.m_polygon->m_scaley) < 0.000001
            && lhs.m_size == rhs.m_size        
            && lhs.m_inverse == rhs.m_inverse
            && lhs.m_effectType == rhs.m_effectType
            && lhs.m_effect == rhs.m_effect);//fix me: unsafe code
    }
    return result;
}

ULONG ClipperAlphaMaskCacheKey::UpdateHashValue()
{
    if(m_clipper)
        m_hash_value = ClipperTraits::Hash(*m_clipper);
    else
        m_hash_value = 0;
    return m_hash_value;
}

//////////////////////////////////////////////////////////////////////////////////////////////

// DrawItemHashKey

ULONG DrawItemHashKey::UpdateHashValue()
{
    m_hash_value = 0;//Yes, we would not really hash now, so that test of operator== can be easier
    return m_hash_value;
}

DrawItemHashKey::DrawItemHashKey( const DrawItem& draw_item)
    : m_overlay_key( draw_item.overlay_paint_machine->GetHashKey() )
    , m_clipper_key( draw_item.clipper->GetHashKey() )
    , m_clip_rect(draw_item.clip_rect)
    , m_xsub(draw_item.xsub)
    , m_ysub(draw_item.ysub)
    , m_fBody(draw_item.fBody)
    , m_fBorder(draw_item.fBorder)
{
    for(int i=0;i<countof(m_switchpts);i++)
        m_switchpts[i] = draw_item.switchpts[i];
}

bool DrawItemHashKey::operator==( const DrawItemHashKey& key ) const
{
    return (this==&key) || (
        *m_overlay_key==*key.m_overlay_key && 
        m_clipper_key==key.m_clipper_key &&
        (m_clip_rect == key.m_clip_rect)==TRUE &&
        m_xsub == key.m_xsub &&
        m_ysub == key.m_ysub &&
        m_fBody == key.m_fBody &&
        m_fBorder == key.m_fBorder &&
        !memcmp(m_switchpts, key.m_switchpts, sizeof(m_switchpts)));
}

//////////////////////////////////////////////////////////////////////////////////////////////

// GroupedDrawItemsHashKey

ULONG GroupedDrawItemsHashKey::UpdateHashValue()
{
    m_hash_value = 0;//Yes, we would not really hash now, so that test of operator== can be easier
    return m_hash_value;
}

bool GroupedDrawItemsHashKey::operator==( const GroupedDrawItemsHashKey& key ) const
{
    if (this==&key)
    {
        return true;
    }
    else if ( m_key->GetCount()!=key.m_key->GetCount() || m_clip_rect!=key.m_clip_rect)
    {
        return false;
    }
    else
    {
        for( unsigned i=0;i<m_key->GetCount();i++)
        {
            if( !(*m_key->GetAt(i) == *key.m_key->GetAt(i)) )
                return false;
        }
        return true;
    }
    return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////

// CacheManager

struct Caches
{
public:
    Caches()
    {
        s_bitmap_cache = NULL;
        s_clipper_alpha_mask_cache = NULL;

        s_text_info_cache = NULL;
        s_word_mru_cache = NULL;
        s_path_data_mru_cache = NULL;
        s_scan_line_data_2_mru_cache = NULL;
        s_overlay_no_blur_mru_cache = NULL;
        s_overlay_mru_cache = NULL;

        s_scan_line_data_mru_cache = NULL;
        s_overlay_no_offset_mru_cache = NULL;
		
        s_subpixel_variance_cache = NULL;
        s_ass_tag_list_cache = NULL;
    }
    ~Caches()
    {
        delete s_bitmap_cache;
        delete s_clipper_alpha_mask_cache;

        delete s_text_info_cache;
        delete s_word_mru_cache;
        delete s_path_data_mru_cache;
        delete s_scan_line_data_2_mru_cache;
        delete s_overlay_no_blur_mru_cache;
        delete s_overlay_mru_cache;

        delete s_scan_line_data_mru_cache;
        delete s_overlay_no_offset_mru_cache;

        delete s_subpixel_variance_cache;
        delete s_ass_tag_list_cache;
    }
public:
    BitmapMruCache* s_bitmap_cache;
    ClipperAlphaMaskMruCache* s_clipper_alpha_mask_cache;

    TextInfoMruCache* s_text_info_cache;
    AssTagListMruCache* s_ass_tag_list_cache;

    ScanLineDataMruCache* s_scan_line_data_mru_cache;
    OverlayNoOffsetMruCache* s_overlay_no_offset_mru_cache;

    OverlayMruCache* s_subpixel_variance_cache;
    OverlayMruCache* s_overlay_mru_cache;
    OverlayNoBlurMruCache* s_overlay_no_blur_mru_cache;
    PathDataMruCache* s_path_data_mru_cache;
    ScanLineData2MruCache* s_scan_line_data_2_mru_cache;
    CWordMruCache* s_word_mru_cache;
};

static Caches s_caches;

OverlayMruCache* CacheManager::GetOverlayMruCache()
{
    if(s_caches.s_overlay_mru_cache==NULL)
    {
        s_caches.s_overlay_mru_cache = new OverlayMruCache(OVERLAY_CACHE_ITEM_NUM);
    }
    return s_caches.s_overlay_mru_cache;
}

PathDataMruCache* CacheManager::GetPathDataMruCache()
{
    if (s_caches.s_path_data_mru_cache==NULL)
    {
        s_caches.s_path_data_mru_cache = new PathDataMruCache(PATH_CACHE_ITEM_NUM);
    }
    return s_caches.s_path_data_mru_cache;
}

CWordMruCache* CacheManager::GetCWordMruCache()
{
    if(s_caches.s_word_mru_cache==NULL)
    {
        s_caches.s_word_mru_cache = new CWordMruCache(WORD_CACHE_ITEM_NUM);
    }
    return s_caches.s_word_mru_cache;
}

OverlayNoBlurMruCache* CacheManager::GetOverlayNoBlurMruCache()
{
    if(s_caches.s_overlay_no_blur_mru_cache==NULL)
    {
        s_caches.s_overlay_no_blur_mru_cache = new OverlayNoBlurMruCache(OVERLAY_NO_BLUR_CACHE_ITEM_NUM);
    }
    return s_caches.s_overlay_no_blur_mru_cache;
}

ScanLineData2MruCache* CacheManager::GetScanLineData2MruCache()
{
    if(s_caches.s_scan_line_data_2_mru_cache==NULL)
    {
        s_caches.s_scan_line_data_2_mru_cache = new ScanLineData2MruCache(SCAN_LINE_DATA_CACHE_ITEM_NUM);
    }
    return s_caches.s_scan_line_data_2_mru_cache;
}

OverlayMruCache* CacheManager::GetSubpixelVarianceCache()
{
    if(s_caches.s_subpixel_variance_cache==NULL)
    {
        s_caches.s_subpixel_variance_cache = new OverlayMruCache(SUBPIXEL_VARIANCE_CACHE_ITEM_NUM);
    }
    return s_caches.s_subpixel_variance_cache;    
}

ScanLineDataMruCache* CacheManager::GetScanLineDataMruCache()
{
    if(s_caches.s_scan_line_data_mru_cache==NULL)
    {
        s_caches.s_scan_line_data_mru_cache = new ScanLineDataMruCache(SCAN_LINE_DATA_CACHE_ITEM_NUM);
    }
    return s_caches.s_scan_line_data_mru_cache;
}

OverlayNoOffsetMruCache* CacheManager::GetOverlayNoOffsetMruCache()
{
    if(s_caches.s_overlay_no_offset_mru_cache==NULL)
    {
        s_caches.s_overlay_no_offset_mru_cache = new OverlayNoOffsetMruCache(OVERLAY_NO_BLUR_CACHE_ITEM_NUM);
    }
    return s_caches.s_overlay_no_offset_mru_cache;    
}

AssTagListMruCache* CacheManager::GetAssTagListMruCache()
{
    if(s_caches.s_ass_tag_list_cache==NULL)
    {
        s_caches.s_ass_tag_list_cache = new AssTagListMruCache(ASS_TAG_LIST_CACHE_ITEM_NUM);
    }
    return s_caches.s_ass_tag_list_cache;  
}

TextInfoMruCache* CacheManager::GetTextInfoCache()
{
    if(s_caches.s_text_info_cache==NULL)
    {
        s_caches.s_text_info_cache = new TextInfoMruCache(TEXT_INFO_CACHE_ITEM_NUM);
    }
    return s_caches.s_text_info_cache;
}

ClipperAlphaMaskMruCache* CacheManager::GetClipperAlphaMaskMruCache()
{
    if(s_caches.s_clipper_alpha_mask_cache==NULL)
    {
        s_caches.s_clipper_alpha_mask_cache = new ClipperAlphaMaskMruCache(CLIPPER_ALPHA_MASK_MRU_CACHE);
    }
    return s_caches.s_clipper_alpha_mask_cache;
}

BitmapMruCache* CacheManager::GetBitmapMruCache()
{
    if (s_caches.s_bitmap_cache==NULL)
    {
        s_caches.s_bitmap_cache = new BitmapMruCache(BITMAP_MRU_CACHE_ITEM_NUM);
    }
    return s_caches.s_bitmap_cache;
}
