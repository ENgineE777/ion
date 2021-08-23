
#ifndef _CEGUIFont_h_
#define _CEGUIFont_h_

//#include "CEGUI/Base.h"
//#include "CEGUI/PropertySet.h"
//#include "CEGUI/EventSet.h"
//#include "CEGUI/String.h"
//#include "CEGUI/XMLSerializer.h"
#include "FontGlyph.h"

#include <map>


namespace CEGUI
{
/*!
\brief
    Class that encapsulates a typeface.

    A Font object is created for each unique typeface required.
    The Font class provides methods for loading typefaces from various sources,
    and then for outputting text via the Renderer object.

    This class is not specific to any font renderer, it just provides the
    basic interfaces needed to manage fonts.
*/
class Font
{
public:
    	        
    //! Destructor.
    virtual ~Font();

    //! Return the string holding the font name.
    const std::string& getName() const;

    //! Return the type of the font.
    const std::string& getTypeName() const;

    //! Return the filename of the used font.
    const std::string& getFileName() const;
    
    /*!
    \brief
        Return whether this Font can draw the specified code-point

    \param cp
        utf32 code point that is the subject of the query.

    \return
        true if the font contains a mapping for code point \a cp,
        false if it does not contain a mapping for \a cp.
    */
    bool isCodepointAvailable(dword cp) const
    { return (d_cp_map.find(cp) != d_cp_map.end()); }

    /*!
    \brief
        Draw text into a specified area of the display.

    \param buffer
        GeometryBuffer object where the geometry for the text be queued.

    \param text
        String object containing the text to be drawn.

    \param position
        Reference to a Vector2 object describing the location at which the text
        is to be drawn.

    \param clip_rect
        Rect object describing the clipping area for the drawing.
        No drawing will occur outside this Rect.

    \param colours
        ColourRect object describing the colours to be applied when drawing the
        text.  NB: The colours specified in here are applied to each glyph,
        rather than the text as a whole.

    \param space_extra
        Number of additional pixels of spacing to be added to space characters.

    \param x_scale
        Scaling factor to be applied to each glyph's x axis, where 1.0f is
        considered to be 'normal'.

    \param y_scale
        Scaling factor to be applied to each glyph's y axis, where 1.0f is
        considered to be 'normal'.

    \return
        The x co-ord where subsequent text should be rendered to ensure correct
        positioning (which is not possible to determine accurately by using the
        extent measurement functions).
    */

    /*float drawText(GeometryBuffer& buffer, const String& text,
                   const Vector2f& position, const Rectf* clip_rect,
                   const ColourRect& colours, const float space_extra = 0.0f,
                   const float x_scale = 1.0f, const float y_scale = 1.0f) const;*/

    /*!
    \brief
        Set the native resolution for this Font

    \param size
        Size object describing the new native screen resolution for this Font.
    */
    //void setNativeResolution(const Sizef& size);

    /*!
    \brief
        Return the native display size for this Font.  This is only relevant if
        the Font is being auto-scaled.

    \return
        Size object describing the native display size for this Font.
    */
    //const Sizef& getNativeResolution() const;

    /*!
    \brief
        Enable or disable auto-scaling for this Font.

    \param auto_scaled
        AutoScaledMode describing how this font should be auto scaled

    \see AutoScaledMode
    */
    //void setAutoScaled(const AutoScaledMode auto_scaled);

    /*!
    \brief
        Checks whether this font is being auto-scaled and how.

    \return
        AutoScaledMode describing how this font should be auto scaled
    */
    //AutoScaledMode getAutoScaled() const;

    /*!
    \brief
        Notify the Font that the display size may have changed.

    \param size
        Size object describing the display resolution
    */
    //virtual void notifyDisplaySizeChanged(const Sizef& size);

    /*!
    \brief
        Return the pixel line spacing value for.

    \param y_scale
        Scaling factor to be applied to the line spacing, where 1.0f
        is considered to be 'normal'.

    \return
        Number of pixels between vertical base lines, i.e. The minimum
        pixel space between two lines of text.
    */
    float getLineSpacing(float y_scale = 1.0f) const
    { return d_height * y_scale; }

    /*!
    \brief
        return the exact pixel height of the font.

    \param y_scale
        Scaling factor to be applied to the height, where 1.0f
        is considered to be 'normal'.

    \return
        float value describing the pixel height of the font without
        any additional padding.
    */
    float getFontHeight(float y_scale = 1.0f) const
    { return (d_ascender - d_descender) * y_scale; }

    /*!
    \brief
        Return the number of pixels from the top of the highest glyph
        to the baseline

    \param y_scale
        Scaling factor to be applied to the baseline distance, where 1.0f
        is considered to be 'normal'.

    \return
        pixel spacing from top of front glyphs to baseline
    */
    float getBaseline(float y_scale = 1.0f) const
    { return d_ascender * y_scale; }

    /*!
    \brief
        Return the pixel width of the specified text if rendered with
        this Font.

    \param text
        String object containing the text to return the rendered pixel
        width for.

    \param x_scale
        Scaling factor to be applied to each glyph's x axis when
        measuring the extent, where 1.0f is considered to be 'normal'.

    \return
        Number of pixels that \a text will occupy when rendered with
        this Font.

    \note
        The difference between the advance and the extent of a text string is
        important for numerous reasons. Picture some scenario where a glyph
        has a swash which extends way beyond the subsequent glyph - the text
        extent of those two glyphs is to the end of the swash on the first glyph
        whereas the advance of those two glyphs is to the start of a theoretical
        third glyph - still beneath the swash of the first glyph.
        The difference can basically be summarised as follows:
        - the extent is the total rendered width of all glyphs in the string.
        - the advance is the width to the point where the next character would
          have been drawn.

    \see getTextAdvance
    */
    float getTextExtent(const std::string& text, float x_scale = 1.0f);

    /*!
    \brief
        Return pixel advance of the specified text when rendered with this Font.

    \param text
        String object containing the text to return the pixel advance for.

    \param x_scale
        Scaling factor to be applied to each glyph's x axis when
        measuring the advance, where 1.0f is considered to be 'normal'.

    \return
        pixel advance of \a text when rendered with this Font.

    \note
        The difference between the advance and the extent of a text string is
        important for numerous reasons. Picture some scenario where a glyph
        has a swash which extends way beyond the subsequent glyph - the text
        extent of those two glyphs is to the end of the swash on the first glyph
        whereas the advance of those two glyphs is to the start of a theoretical
        third glyph - still beneath the swash of the first glyph.
        The difference can basically be summarised as follows:
        - the extent is the total rendered width of all glyphs in the string.
        - the advance is the width to the point where the next character would
          have been drawn.

    \see getTextExtent
    */
    float getTextAdvance(const std::string& text, float x_scale = 1.0f);

    /*!
    \brief
        Return the index of the closest text character in String \a text
        that corresponds to pixel location \a pixel if the text were rendered.

    \param text
        String object containing the text.

    \param pixel
        Specifies the (horizontal) pixel offset to return the character
        index for.

    \param x_scale
        Scaling factor to be applied to each glyph's x axis when measuring
        the text extent, where 1.0f is considered to be 'normal'.

    \return
        Returns a character index into String \a text for the character that
        would be rendered closest to horizontal pixel offset \a pixel if the
        text were to be rendered via this Font.  Range of the return is from
        0 to text.length(), so may actually return an index past the end of
        the string, which indicates \a pixel was beyond the last character.
    */
    size_t getCharAtPixel(const std::string& text, float pixel,
                          float x_scale = 1.0f)
    { return getCharAtPixel(text, 0, pixel, x_scale); }

    /*!
    \brief
        Return the index of the closest text character in String \a text,
        starting at character index \a start_char, that corresponds
        to pixel location \a pixel if the text were to be rendered.

    \param text
        String object containing the text.

    \param start_char
        index of the first character to consider.  This is the lowest
        value that will be returned from the call.

    \param pixel
        Specifies the (horizontal) pixel offset to return the character
        index for.

    \param x_scale
        Scaling factor to be applied to each glyph's x axis when measuring
        the text extent, where 1.0f is considered to be 'normal'.

    \return
        Returns a character index into String \a text for the character that
        would be rendered closest to horizontal pixel offset \a pixel if the
        text were to be rendered via this Font.  Range of the return is from
        0 to text.length(), so may actually return an index past the end of
        the string, which indicates \a pixel was beyond the last character.
    */
    size_t getCharAtPixel(const std::string& text, size_t start_char, float pixel,
                          float x_scale = 1.0f);

    /*!
    \brief
        Sets the default resource group to be used when loading font data

    \param resourceGroup
        String describing the default resource group identifier to be used.

    \return
        Nothing.
    */
    static void setDefaultResourceGroup(const std::string& resourceGroup)
    { d_defaultResourceGroup = resourceGroup; }

    /*!
    \brief
        Returns the default resource group currently set for Fonts.

    \return
        String describing the default resource group identifier that will be
        used when loading font data.
    */
    static const std::string& getDefaultResourceGroup()
    { return d_defaultResourceGroup; }

    /*!
    \brief
        Writes an xml representation of this Font to \a out_stream.

    \param xml_stream
        Stream where xml data should be output.

    \return
        Nothing.
    */
    //void writeXMLToStream(XMLSerializer& xml_stream) const;

    /*!
    \brief
        Return a pointer to the glyphDat struct for the given codepoint,
        or 0 if the codepoint does not have a glyph defined.

    \param codepoint
        utf32 codepoint to return the glyphDat structure for.

    \return
        Pointer to the glyphDat struct for \a codepoint, or 0 if no glyph
        is defined for \a codepoint.
    */
    FontGlyph* getGlyphData(dword codepoint);

protected:
    //! Constructor.
    Font(const std::string& name, const std::string& type_name, const std::string& filename,
         const std::string& resource_group,
         const Vector2& native_res);

    /*!
    \brief
        This function prepares a certain range of glyphs to be ready for
        displaying. This means that after returning from this function
        glyphs from d_cp_map[start_codepoint] to d_cp_map[end_codepoint]
        should have their d_image member set. If there is an error
        during rasterisation of some glyph, it's okay to leave the
        d_image field set to NULL, in which case such glyphs will
        be skipped from display.
    \param start_codepoint
        The lowest codepoint that should be rasterised
    \param end_codepoint
        The highest codepoint that should be rasterised
    */
    virtual void rasterise(dword start_codepoint, dword end_codepoint) const;

    //! Update the font as needed, according to the current parameters.
    virtual void updateFont() = 0;

    //! implementaion version of writeXMLToStream.
    //virtual void writeXMLToStream_impl(XMLSerializer& xml_stream) const = 0;

    //! Register all properties of this class.
    void addFontProperties();

    //! event trigger function for when the font rendering size changes.
	//virtual void onRenderSizeChanged(FontEventArgs& args);

    /*!
    \brief
        Set the maximal glyph index. This reserves the respective
        number of bits in the d_glyphPageLoaded array.
    */
    void setMaxCodepoint(dword codepoint);

    //! finds FontGlyph in map and returns it, or 0 if none.
    virtual FontGlyph* findFontGlyph(const dword codepoint);

    //! Name of this font.
    std::string d_name;
    //! Type name string for this font (not used internally)
    std::string d_type;
    //! Name of the file used to create this font (font file or imagset)
    std::string d_filename;
    //! Name of the font file's resource group.
    std::string d_resourceGroup;
    //! Holds default resource group for font loading.
    static std::string d_defaultResourceGroup;

    //! maximal font ascender (pixels above the baseline)
    float d_ascender;
    //! maximal font descender (negative pixels below the baseline)
    float d_descender;
    //! (ascender - descender) + linegap
    float d_height;

    //! which mode should we use for auto-scaling
    //AutoScaledMode d_autoScaled;

    //! native resolution for this Font.
    Vector2 d_nativeResolution;
    //! current horizontal scaling factor.
    float d_horzScaling;
    //! current vertical scaling factor.
    float d_vertScaling;

    //! Maximal codepoint for font glyphs
    dword d_maxCodepoint;

    /*!
    \brief
        This bitmap holds information about loaded 'pages' of glyphs.
        A glyph page is a set of 256 codepoints, starting at 256-multiples.
        For example, the 1st glyph page is 0-255, fourth is 1024-1279 etc.
        When a specific glyph is required for painting, the corresponding
        bit is checked to see if the respective page has been rasterised.
        If not, the rasterise() method is invoked, which prepares the
        glyphs from the respective glyph page for being painted.

        This array is big enough to hold at least max_codepoint bits.
        If this member is NULL, all glyphs are considered pre-rasterised.
    */
    dword* d_glyphPageLoaded;

    //! Definition of CodepointMap type.
    typedef std::map<dword, FontGlyph, std::less<dword>> CodepointMap;

    //! Contains mappings from code points to Image objects
    mutable CodepointMap d_cp_map;
};



} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif


#endif  // end of guard _CEGUIFont_h_
