#include <unordered_map>
#include <array>
#include <bitset>
#include <random> // TODO: replace with cryptographically secure rng

#define KEYSIZE 32
#define REGCOUNT 16
#define REGSIZE 32


std::random_device rdev;
std::mt19937 rng(rdev());
std::bernoulli_distribution bdist(0.5);

class enc_key {
public:
    std::bitset<KEYSIZE> key;
    bool used = false;

    enc_key() {
        rgen();
    }

    void rgen() {
        for (int n = 0; n < KEYSIZE; n++) {
            key[n] = bdist(rng);
        }
    }
};

// key lookup table, id -> key, append new keys as needed
int ktable_nfilled = 0;
std::vector<enc_key> ktable;


class mem_element {
public:
    std::vector<bool> enc_data;
    int key_id = -1;

    mem_element() {
        key_id = rgen_key_id();
    }

    void unlock() {
        for (int i = 0; i < enc_data.size(); i++) {
            enc_data[i] ^= ktable[key_id].key[i % ktable[key_id].key.size()];
        }
        ktable_nfilled++;
    }

    void lock() {
        ktable[key_id].rgen();
        rgen_key_id();
        for (int i = 0; i < enc_data.size(); i++) {
            enc_data[i] ^= ktable[key_id].key[i % ktable[key_id].key.size()];
        }
    }

private:
    void rgen_key_id() {
        if (key_id != -1) {
            ktable[key_id].used = false;
        }
        if (ktable_nfilled == ktable.size()) {
            for (int i = 0; i < ktable.size() * 2; i++) {
                enc_key k;
                ktable.push_back(k);
            }
            key_id = ktable_nfilled;
        } else {
            std::uniform_int_distribution<std::mt19937::result_type> udist(0, ktable.size());
            do {
                key_id = udist(rng);
            }
            while (ktable[key_id].used == true);
        }
        ktable[key_id].used = true;
    }
};

// (id, mem)
// note: id is randomized by assembler
std::unordered_map<int, mem_element> mem;


/*

example program:

store instruction is special, includes initial xor key
store %1, "echo helloworld"
opcode#, 1 byte, mem_elem#, 4 bytes size, para encrypted, 4 bytes size, xor key

.start
store %1, "echo helloworld" -> store string in db
oscall shell, %1 -> call os function
oscall printf, "%s", %1
store %2, "wow"
store %3, .start -> store ip offset in db
mov %1, %2 -> copy db elem
jeq %1, %2, %3 -> jump if eq to ip offset

fib:

fib:

store %1, 1
store %2, 1
oscall scanf, "%i", %3
store %4, 0
store %5, 0
.loop
jeq %3, %4, .end
add %5, %1
add %5, %2
oscall printf, "%c", %5
mov %1, %2
mov %2, %5
jmp .loop
.end

*/
