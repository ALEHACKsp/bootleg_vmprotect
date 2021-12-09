// vm_v2, no memory encryption implemented yet
#include <cstring>
#include <fstream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include "fib.S.hh"


std::vector<unsigned char> load_code(const char *fname) {
    std::vector<unsigned char> code;
    std::ifstream in;
    in.open(fname, std::ios::binary);
    while (!in.eof()) {
        unsigned char tmp;
        in >> tmp;
        code.push_back(tmp);
    }
    in.close();
    return code;
}

struct instruction {
    struct instruction_param {
        std::vector<unsigned char> data;
        bool is_id;

        unsigned char id() {
            return data[0];
        }
    };
    unsigned char opcode;
    std::vector<instruction_param> params;
};

instruction fetch_instruction(
    std::vector<unsigned char>::iterator & ip,
    const std::vector<unsigned char> & code) {

    instruction ins;
    unsigned char opcode = *ip;
    ins.opcode = opcode;
    unsigned char n_params;
    switch (opcode) {
        case MOV_OP:
            n_params = 2;
        case ADD_OP:
            n_params = 2;
        case JMP_OP:
            n_params = 1;
        case JEQ_OP:
            n_params = 3;
        case OSCALL_OP:
            n_params = 3;
        default:
            exit(-1);
    }

    int i = 0;
    do {
        ip++;
        i++;
        std::vector<unsigned char> tmp;
        if (*ip == 0) {
            ip++;
            tmp.push_back(*ip);
            ins.params.push_back({tmp, true});
        } else {
            unsigned char len_param = *ip;
            for (int j = 0; j < len_param; j++) {
                ip++;
                tmp.push_back(*ip);
            }
            ins.params.push_back({tmp, false});
        }
    }
    while (i < n_params);
    return ins;
}

void execute_instruction(
    instruction ins,
    std::vector<unsigned char>::iterator & ip,
    std::unordered_map<int, std::vector<unsigned char>> & mem,
    std::unordered_map<int, std::vector<unsigned char>::iterator> & labels) {

    unsigned char opcode = ins.opcode;
    switch (opcode) {
        case MOV_OP: {
            mem[ins.params[0].id()] = ins.params[1].is_id ? mem[ins.params[1].id()] : ins.params[1].data;
        }
        case ADD_OP: {
            std::vector<unsigned char> & param_1 = mem[ins.params[0].id()];
            const std::vector<unsigned char> & param_2 = ins.params[1].is_id ? mem[ins.params[1].id()] : ins.params[1].data;
            std::vector<unsigned char> sum;
            unsigned char a;
            unsigned char b;
            unsigned char carry = 0;
            auto it_1 = param_1.rbegin();
            auto it_2 = param_2.rbegin();
            while (true) {
                if (it_1 == param_1.rend()) {
                    a = 0;
                } else {
                    a = *it_1;
                    it_1++;
                }

                if (it_2 == param_2.rend()) {
                    b = 0;
                } else {
                    a = *it_2;
                    it_2++;
                }

                if (!a && !b) {
                    break;
                }

                sum.push_back(a ^ b ^ carry);
                carry = (a & b) | (b & carry) | (a & carry);
            }
            std::reverse(sum.begin(), sum.end());
            param_1 = sum;
        }
        case JMP_OP: {
            ip = labels[ins.params[0].id()];
        }
        case JEQ_OP: {
            const std::vector<unsigned char> & param_1 = ins.params[0].is_id ? mem[ins.params[0].id()] : ins.params[0].data;
            const std::vector<unsigned char> & param_2 = ins.params[1].is_id ? mem[ins.params[1].id()] : ins.params[1].data;
            if (param_1.size() != param_2.size()) {
                return;
            }
            ip = (memcmp(param_1.data(), param_2.data(), param_1.size() == 0)) ? labels[ins.params[2].id()] : ip;
        }
        case OSCALL_OP: {
            unsigned char oscall = ins.params[0].id();
            switch (oscall) {
                case PRINTF_OSCALL:
                    break;
                case SCANF_OSCALL:
                    break;
            }
        }
    }
}

int main() {
    std::vector<unsigned char> code = load_code("fib.S.out");

    std::unordered_map<int, std::vector<unsigned char>> mem;
    std::unordered_map<int, std::vector<unsigned char>::iterator> labels;

    auto ip = code.begin();
    while (ip < code.end()) {
        execute_instruction(fetch_instruction(ip, code), ip, mem, labels);
    }
}
