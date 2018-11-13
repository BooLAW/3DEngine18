#include "MaterialLoader.h"
#include "Component.h"
#include "ComponentMaterial.h"
#include "Material.h"
#include "LoadManager.h"



MaterialLoader::MaterialLoader()
{
	unique_path = "";
}

bool MaterialLoader::Start()
{
	iluInit();
	ilInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	return true;
}


MaterialLoader::~MaterialLoader()
{

}

ComponentMaterial* MaterialLoader::LoadPNG(const char* file_name)
{
	
	GLuint textureID;
	ILuint imageID;

	ILboolean success = true;
	ILenum error;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	ComponentMaterial* comp = new ComponentMaterial(nullptr);
	comp->data = new Material();		
	comp->SetActive(true);
	comp->Enable();
	comp->tex_path.append(file_name);
	success = ilLoadImage(file_name);


	if (success)
	{
		CONSOLE_LOG_INFO("Loading texture: %s",App->loading_manager->GetFileName(file_name).c_str());
		// If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		// Convert the image into a suitable format to work with
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		comp->data->width = ilGetInteger(IL_IMAGE_WIDTH);
		comp->data->height = ilGetInteger(IL_IMAGE_HEIGHT);
		comp->type = ComponentType::MATERIAL;
		// Quit out if we failed the conversion
		if (!success)
		{
			error = ilGetError();
			CONSOLE_LOG_ERROR("Image conversion failed - IL reports error: - s%", iluErrorString(error));
			exit(-1);
		}
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Generate a new texture
		glGenTextures(1, &textureID);

		// Bind the texture to a name
		glBindTexture(GL_TEXTURE_2D, textureID);

		comp->data->textures_id = textureID;

		// Set texture clamping method
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// Set texture interpolation method to use linear interpolation (no MIPMAPS)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Specify the texture specification
		glTexImage2D(GL_TEXTURE_2D, 				// Type of texture
			0,				// Pyramid level (for mip-mapping) - 0 is the top level
			GL_RGB,	// Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
			ilGetInteger(IL_IMAGE_WIDTH),	// Image width
			ilGetInteger(IL_IMAGE_HEIGHT),	// Image height
			0,				// Border width in pixels (can either be 1 or 0)
			ilGetInteger(IL_IMAGE_FORMAT),	// Format of image pixel data
			GL_UNSIGNED_BYTE,		// Image data type
			ilGetData());			// The actual image data itself

		CONSOLE_LOG_INFO("Texture Loaded to VRAM");
	}
	else // If we failed to open the image file in the first place...
	{
		textureID = 0;
		//error = ilGetError();
		CONSOLE_LOG_WARNING("%s not found", App->loading_manager->GetFileName(file_name).c_str());
		//exit(-1);
	}
	//ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we

	return comp;
	
	
}
	



