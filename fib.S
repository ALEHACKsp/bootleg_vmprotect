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
