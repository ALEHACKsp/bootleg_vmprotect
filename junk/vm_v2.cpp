// vm_v2, no memory encryption implemented yet
#include <vector>
#include <unordered_map>
#include "fib.S.hh"


std::vector<unsigned char> load_code(const char *fname) {
    std::vector<unsigned char> code;
    ifstream in;
    in.open(fname, ios::binary);
    while (!if.eof()) {
        unsigned char tmp;
        in >> tmp;
        code.push_back(tmp);
    }
    in.close()
    return code;
}

std::vector<std::pair<bool, std::vector<unsigned char>>> fetch_instruction(
    std::vector<unsigned char>::iterator & ip,
    std::vector<unsigned char> & code,
    std::unordered_map<int, std::vector<unsigned char>::iterator> & labels) {

    unsigned char opcode = *ip;
    std::vector<std::vector<unsigned char>> instruction;
    instruction.push_back(std::make_pair(false, opcode));
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
            ip++;
            labels[*ip] = ++ip;
            return;
    }
    int i = 0;
    do {
        ip++;
        i++;
        if (*ip == 0) {
            ip++;
            instruction.push_back(std::make_pair(true, *ip));
        }
        else {
            unsigned char param_len = *ip;
            for (int j = 0; j < param_len; j++) {
                ip++;
                instruction.push_back(std::make_pair(false, *ip));
            }
        }
    }
    while (i < n_params);
    return instruction;
}

void call_oscall(unsigned char oscall, std::vector<std::vector<unsigned char>> & params) {
    switch (oscall) {
        case PRINTF_OSCALL:
            break;
        case SCANF_OSCALL:
            break;
    }
}

void execute_instruction(
    std::vector<std::pair<bool, std::vector<unsigned char>>> & instruction,
    std::unordered_map<int, std::vector<unsigned char>> & mem,
    std::unordered_map<int, std::vector<unsigned char>::iterator> & labels,
    unsigned char>::iterator & ip) {

    // unsigned char opcode = instruction[0].second;
    // switch (opcode) {
    //     case MOV_OP:
    //         mem[instruction[1].first] = instruction[2].first ? mem[instruction[2].second] : instruction[2].second];
    //     case ADD_OP:
    //         mem[instruction[1].first] += instruction[2].first ? mem[instruction[2].second] : instruction[2].second];
    //     case JMP_OP:
    //         ip = labels[instruction[1].first];
    //     case JEQ_OP:
    //         ip = labels[instruction[1].first] == labels[instruction[2].first] ? labels[instruction[3].first] : ip;
    //     case OSCALL_OP:
    //         std::vector<std::vector<unsigned char>> params;
    //         for (auto param = ++instruction.begin(); param != instruction.end(); param++) {
    //             params.push_back(*param[1].second);
    //         }
    //         call_oscall(instruction[1].second, params);
    // }

    unsigned char opcode = instruction[0].second;
    switch (opcode) {
        case MOV_OP:
            mem[instruction[1].first] = instruction[2].first ? mem[instruction[2].second] : instruction[2].second];
        case ADD_OP:
            mem[instruction[1].first] += instruction[2].first ? mem[instruction[2].second] : instruction[2].second];
        case JMP_OP:
            ip = labels[instruction[1].first];
        case JEQ_OP:
            ip = labels[instruction[1].first] == labels[instruction[2].first] ? labels[instruction[3].first] : ip;
        case OSCALL_OP:
            std::vector<std::vector<unsigned char>> params;
            for (auto param = ++instruction.begin(); param != instruction.end(); param++) {
                params.push_back(*param[1].second);
            }
            call_oscall(instruction[1].second, params);
    }
}

int main() {
    std::vector<unsigned char> code = load_code("fib.S.out");

    std::unordered_map<int, std::vector<unsigned char>> mem;
    std::unordered_map<int, std::vector<unsigned char>::iterator> labels;

    auto ip = code.begin();
    while (ip < code.end()) {
        execute_instruction(fetch_instruction(ip, code, labels), mem, labels, ip);
    }
}
