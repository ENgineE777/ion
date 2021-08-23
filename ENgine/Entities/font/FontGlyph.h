

#pragma once


#include "core/core.h"

namespace CEGUI
{
	class FontGlyph
	{
		public:

		FontGlyph(float advance = 0.0f, Texture* image = 0, bool valid = false);
		
		Texture* getImage();		

		//! Return the scaled pixel size of the glyph.
		//Sizef getSize(float x_scale, float y_scale) const
		//{ return Sizef(getWidth(x_scale), getHeight(y_scale)); }
		
		float getWidth(float x_scale);				
		float getHeight(float y_scale);						
		float getRenderedAdvance(float x_scale);				
		float getAdvance(float x_scale = 1.0);				
		void setAdvance(float advance);				
		void setImage(Texture* image);				
		void setValid(bool valid);			
		bool isValid() const;		

		private:
			
		//! The image which will be rendered for this glyph.
		Texture* d_image;

	    //! Amount to advance the pen after rendering this glyph
	    float d_advance;

	    //! says whether this glyph info is actually valid
	    bool d_valid;
	};

} // End of  CEGUI namespace section
