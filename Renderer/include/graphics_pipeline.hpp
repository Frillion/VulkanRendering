#ifndef PIPELINE_HEADER
#define PIPELINE_HEADER

#include <string>
#include <vector>
namespace FRI{

class Pipeline{
public:
    Pipeline(const std::string& vertex_shader_path, const std::string& fragment_shader_path);
private:
    static std::vector<char> parse_shader(const std::string& shader_source);
};

}

#endif // !PIPELINE_HEADER
