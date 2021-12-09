import sys
import secrets
import random
import struct
import string

# 255 memory locations + labels + oscalls + opcodes combined
ID_CAP = 255
JUNK_PERCENTAGE = 0.25

def main():
    lines = None
    with open(sys.argv[1], "r") as f:
        lines = f.readlines()

    # NOTE: instruction parameters must not exceed > 255 bytes in length
    # assembled instruction:
    # - 1 byte opcode, 1 byte length para 1, para 1, 1 byte length para 2, ...
    # - opcode determines n of paras when parsing
    # no length specified -> treat as ID

    oscall_l = ["scanf", "printf"]
    opcodes = {}
    oscalls = {}
    mem_elems = {}
    labels = {}

    ids = random.sample(range(0, ID_CAP), ID_CAP)
    curr_id_indx = 0
    assembled = bytes()


    line_indx = 0
    for line in lines:
        line = line.strip()
        if random.random() < JUNK_PERCENTAGE:
            for i in range(0, random.randint(0, int(JUNK_PERCENTAGE * 8))):
                mem_l = ids[curr_id_indx]
                curr_id_indx += 1
                junk = [f"add %{mem_l}, 0", f"mov %{mem_l}, {mem_l}", f"sub %{mem_l}, 0", f"store %{mem_l} {''.join(random.choice(string.ascii_letters + string.digits + string.punctuation) for i in range(random.randint(0, JUNK_PERCENTAGE * 8)))}"]
                lines.insert(line_indx + 1, random.choice(junk))
        if line[0] == ".":
            id = ids[curr_id_indx]
            curr_id_indx += 1
            labels[line] = id
        line_indx += 1

    for line in lines:
        line = line.strip()
        if line[0] == ".":
            continue

        # note: use re for this
        # https://stackoverflow.com/questions/2785755/how-to-split-but-ignore-separators-in-quoted-strings-in-python
        line = line.split(" ") # todo: fix this to handle "" strings
        opcode = line[0]
        paras = line[1:]

        if opcode not in opcodes:
            opcodes[opcode] = secrets.token_bytes(1)

        assembled_paras = []

        for para in paras:

            if para[-1] == ",":
                para = para[:-1]

            if para[0] == "%":
                id = ids[curr_id_indx]
                curr_id_indx += 1
                mem_elems[para] = id
                # todo: implement register sub
                # todo: implement parameter info
                assembled_paras.append((0, struct.pack(">B", id)))

            elif para[0] == "\"":
                # todo: obfuscate with xor or shifts at assembletime
                p_para = bytes(para, "ascii")
                assembled_paras.append((len(p_para), p_para))
                # key = secrets.token_bytes(len(p_para))
                # assembled_paras.append((len(p_para), bytes(a ^ b for a, b in zip(p_para, key)) + key))

            elif para.isnumeric():
                # todo: figure out a cleaner way to obfuscate constant values bc this isnt too pretty
                __tmp = struct.pack(">I", int(para))
                assembled_paras.append((len(__tmp), __tmp))
                # key = secrets.token_bytes(len(__tmp))
                # assembled_paras.append((len(__tmp), bytes(a ^ b for a, b in zip(__tmp, key)) + key))

            elif para[0] == ".":
                assembled_paras.append((0, struct.pack(">B", labels[para])))

            elif opcode == "oscall" and para in oscall_l:
                id = ids[curr_id_indx]
                curr_id_indx += 1
                oscalls[para] = id
                assembled_paras.append((0, struct.pack(">B", id)))

        assembled += opcodes[opcode]
        for assembled_para in assembled_paras:
            # note: use this for var-length parameters, but 0 for id paras
            l_para = assembled_para[0]
            if (l_para > 255):
                raise Exception

            if l_para == 0:
                assembled += b"\x00" + assembled_para[1]
            else:
                assembled += struct.pack(">B", l_para) + assembled_para[1]

    with open(f"{sys.argv[1]}.out", "wb") as f:
        f.write(assembled)

    with open(f"{sys.argv[1]}.hh", "w") as f:
        for opcode, id in opcodes.items():
            f.write(f"#define {opcode.upper()}_OP {int.from_bytes(id, 'big')}\n")
        for oscall, id in oscalls.items():
            f.write(f"#define {oscall.upper()}_OSCALL {id}\n")


if __name__ == "__main__":
    main()
