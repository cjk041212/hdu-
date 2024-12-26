
while(1):
    n = int(input())
    if n == -1: break
    f = [0] * (n + 1)
    f[0] = 1
    for i in range(1,n+1):
        f[i] = f[i-1] * (4 * i - 2) / (i + 1)
    print(int(f[n]))