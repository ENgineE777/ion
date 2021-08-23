
#include "FontGlyph.h"
#include "core/Render/Textures/Texture.h"

namespace CEGUI
{
	FontGlyph::FontGlyph(float advance, Texture* image, bool valid) :
	        d_image(image),
			d_advance(advance),
			d_valid(valid)
	{}
	
	Texture* FontGlyph::getImage()
	{
		return d_image;
	}

	
	float FontGlyph::getWidth(float x_scale)
	{
		return d_image->GetWidth() * x_scale;
	}
	
	float FontGlyph::getHeight(float y_scale)
	{
		return d_image->GetHeight() * y_scale;
	}
		
	float FontGlyph::getRenderedAdvance(float x_scale)
	{
		return (d_image->GetWidth() /*+ d_image->getRenderedOffset().d_x*/) * x_scale;
	}

	float FontGlyph::getAdvance(float x_scale)
	{
		return d_advance * x_scale;
	}

	void FontGlyph::setAdvance(float advance)
	{
		d_advance = advance;
	}

	void FontGlyph::setImage(Texture* image)
	{
		d_image = image;
	}

	void FontGlyph::setValid(bool valid)
	{
		d_valid = valid;
	}
	
	bool FontGlyph::isValid() const
	{
		return d_valid;
	}
	
} // End of  CEGUI namespace section
