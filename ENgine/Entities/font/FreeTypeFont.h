
#ifndef _CEGUIFreeTypeFont_h_
#define _CEGUIFreeTypeFont_h_

#include "Font.h"
//#include "CEGUI/DataContainer.h"
//#include "CEGUI/BasicImage.h"

#include <ft2build.h>

// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
    Implementation of the Font class interface using the FreeType library.

    This implementation tries to provide maximal support for any kind of
    fonts supported by FreeType. It has been tested on outline font formats
    like TTF and PS as well as on bitmap font formats like PCF and FON.

    Glyphs are rendered dynamically on demand, so a large font with lots
    of glyphs won't slow application startup time.
*/
class FreeTypeFont : public Font
{
public:
    /*!
    \brief
        Constructor for FreeTypeFont based fonts.

    \param font_name
        The name that the font will use within the CEGUI system.

    \param point_size
        Specifies the point size that the font is to be rendered at.

    \param anti_aliased
        Specifies whether the font should be rendered using anti aliasing.

    \param font_filename
        The filename of an font file that will be used as the source for
        glyph images for this font.

    \param resource_group
        The resource group identifier to use when loading the font file
        specified by \a font_filename.

    \param auto_scaled
        Specifies whether the font imagery should be automatically scaled to
        maintain the same physical size (which is calculated by using the
        native resolution setting).

    \param native_horz_res
        The horizontal native resolution value.  This is only significant when
        auto scaling is enabled.

    \param native_vert_res
        The vertical native resolution value.  This is only significant when
        auto scaling is enabled.

	\param specific_line_spacing
        If specified (non-zero), this will be the line spacing that we will
        report for this font, regardless of what is mentioned in the font file
        itself.
    */
    FreeTypeFont(const std::string& font_name, const float point_size,
                 const bool anti_aliased, const std::string& font_filename,
                 const std::string& resource_group = "",
                 const Vector2 native_res = Vector2(640.0f, 480.0f),
                 const float specific_line_spacing = 0.0f);

    //! Destructor.
    ~FreeTypeFont();

    //! return the point size of the freetype font.
    float getPointSize() const;

    //! return whether the freetype font is rendered anti-aliased.
    bool isAntiAliased() const;

    //! return the point size of the freetype font.
    void setPointSize(const float point_size);

    //! return whether the freetype font is rendered anti-aliased.
    void setAntiAliased(const bool anti_alaised);

protected:
    /*!
    \brief
        Copy the current glyph data into \a buffer, which has a width of
        \a buf_width pixels (not bytes).

    \param buffer
        Memory buffer large enough to receive the imagery for the currently
        loaded glyph.

    \param buf_width
        Width of \a buffer in pixels (where each pixel is a argb_t).

    \return
        Nothing.
    */
    void drawGlyphToBuffer(byte* buffer, dword buf_width) const;

    /*!
    \brief
        Return the required texture size required to store imagery for the
        glyphs from s to e
    \param s
        The first glyph in set
    \param e
        The last glyph in set
    */
    dword getTextureSize(CodepointMap::iterator s,
                        CodepointMap::iterator e) const;

    //! Register all properties of this class.
    void addFreeTypeFontProperties();
    //! Free all allocated font data.
    void free();

    //! initialise FontGlyph for given codepoint.
    void initialiseFontGlyph(CodepointMap::iterator cp) const;

    void initialiseGlyphMap();

    // overrides of functions in Font base class.
    FontGlyph* findFontGlyph(const dword codepoint);
    void rasterise(dword start_codepoint, dword end_codepoint) const;
    void updateFont();
    //void writeXMLToStream_impl (XMLSerializer& xml_stream) const;

    //! If non-zero, the overridden line spacing that we're to report.
    float d_specificLineSpacing;
    //! Point size of font.
    float d_ptSize;
    //! True if the font should be rendered as anti-alaised by freeType.
    bool d_antiAliased;
    //! FreeType-specific font handle
    FT_Face d_fontFace;
    
	//! Font file data
	IFileBuffer* d_fontData;

    //! Type definition for TextureVector.
    typedef std::vector<Texture*> TextureVector;
    //! Textures that hold the glyph imagery for this font.
    mutable TextureVector d_glyphTextures;
    
	typedef std::vector<Texture*> ImageVector;

    //! collection of images defined for this font.
    
	mutable ImageVector d_glyphImages;
};

} // End of  CEGUI namespace section

#endif	// end of guard _CEGUIFreeTypeFont_h_
