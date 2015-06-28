#pragma once

namespace glv{

	class Drawable;

	class Parser
	{
	public:

		enum FILE_FORMAT
		{
			STL,
		};

		Parser();
		~Parser();

		std::unique_ptr<MeshDrawable> read(const char* file, FILE_FORMAT format);

	private:

		std::unique_ptr<MeshDrawable> readStl(const char* file);

	};
}
