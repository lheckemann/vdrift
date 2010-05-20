#ifndef _GRAPHICS_CONFIG_H
#define _GRAPHICS_CONFIG_H

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <set>

struct GRAPHICS_CONFIG_SHADER
{
	std::string name; ///< the name of the shader package
	std::string folder; ///< the name of the folder where the shader package is
	std::string defines; ///< space delimited list of #defines to be set
	
	bool Load(std::istream & f, std::ostream & error_output, int & linecount);
};

/// a space-delimited list of string conditions. putting ! before the condition negates it.
struct GRAPHICS_CONFIG_CONDITION
{
	std::set <std::string> positive_conditions;
	std::set <std::string> negated_conditions;
	
	void Parse(const std::string & str);
	bool Satisfied(const std::set <std::string> & conditions) const;
};

struct GRAPHICS_CONFIG_OUTPUT
{
	class SIZE
	{
	private:
		unsigned int value;
		unsigned int fb_div;
		unsigned int fb_mult;
		
	public:
		SIZE() : value(0), fb_div(0), fb_mult(0) {}
		
		unsigned int GetSize(unsigned int framebuffer_size) const
		{
			if (value != 0)
				return value;
			else
			{
				if (fb_div != 0)
					return framebuffer_size / fb_div;
				else if (fb_mult != 0)
					return framebuffer_size / fb_mult;
				else
					return framebuffer_size;
			}
		}
		void Parse(const std::string & str);
	};
	
	std::string name; ///< the name of the output
	SIZE width; ///< sizes can be absolute numbers, "framebuffer", "framebuffer/X" (where X is an int), or "framebuffer*X"
	SIZE height;
	std::string type; ///< can be "2D", "rectangle", "cube", or "framebuffer" -- note that if it's framebuffer, the other fields are ignored
	std::string filter; ///< can be "linear" or "nearest"
	std::string format; ///< can be "RGB", "RGBA", or "depth"
	bool mipmap;
	int multisample; ///< zero indicates no multisampling, any negative number means use the same as the framebuffer (can also specify "framebuffer")
	GRAPHICS_CONFIG_CONDITION conditions;
	
	bool Load(std::istream & f, std::ostream & error_output, int & linecount);
};

struct GRAPHICS_CONFIG_INPUTS
{
	/// this maps texture units to output names (to be used as inputs)
	std::map <unsigned int, std::string> tu;
	
	/// str should be a space-delimited lists of the form tunumber:outputname where tunumber: is optional
	void Parse(const std::string & str);
};

struct GRAPHICS_CONFIG_PASS
{
	std::string camera; ///< the name of the camera
	std::string draw; ///< can be "postprocess" or the name of a drawable layer
	GRAPHICS_CONFIG_INPUTS inputs; ///< assigns outputs of other passes to inputs
	std::string light; ///< the name of the scene-wide directional light
	std::string output; ///< must correspond to a GRAPHICS_CONFIG_OUTPUT
	std::string shader; ///< the name of the shader to use
	bool clear_color; ///< whether or not to clear the color buffer before rendering
	bool clear_depth; ///< whether or not to clear the depth buffer before rendering
	bool write_depth; ///< whether or not to write to the depth buffer during rendering
	bool cull; ///< whether or not to do frustum and distance culling
	GRAPHICS_CONFIG_CONDITION conditions;
	
	bool Load(std::istream & f, std::ostream & error_output, int & linecount);
};

struct GRAPHICS_CONFIG
{
	std::vector <GRAPHICS_CONFIG_SHADER> shaders;
	std::vector <GRAPHICS_CONFIG_OUTPUT> outputs;
	std::vector <GRAPHICS_CONFIG_PASS> passes;
	
	bool Load(const std::string & filename, std::ostream & error_output)
	{
		std::ifstream f(filename.c_str());
		if (!f)
		{
			error_output << "Unable to open graphics config file: " << filename;
			return false;
		}
		return Load(f, error_output);
	}
	
	bool Load(std::istream & f, std::ostream & error_output);
};

#endif