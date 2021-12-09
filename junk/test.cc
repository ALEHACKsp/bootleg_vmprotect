#include <vector>
#include <cstdio>
//
// union instruction_data {
//     std::vector<unsigned char> value;
//     unsigned char id;
//     ~instruction_data() {}
// };
//
// struct instruction {
//     instruction_data data;
//     bool is_id;
// };
//
// struct instruction {
//     union {
//         std::vector<unsigned char> value;
//         unsigned char id;
//     } data;
//     bool is_id;
// }

struct instruction {
    struct instruction_param {
        std::vector<unsigned char> data;
        bool is_id;
    };
    unsigned char opcode;
    std::vector<instruction_param> params;
};



int main() {
    instruction i;
    i.opcode = 32;
    std::vector<unsigned char> data;
    i.params.push_back({data, true});
    i.params[0].is_id = true;
    printf("%i %i\n", i.opcode, i.params[0].is_id);
}
