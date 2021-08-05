# This will be used to generate the gold answers. 

sum=0
n=0
n= int(input())

x0 =int(input())
y0 =int(input())
n = n-1

while(n!=0):
    x1 = int(input())
    y1 = int(input())

    sum += (y1+y0)*(x1-x0)

    x0=x1
    y0=y1
    n-=1


sum/=2

print(sum)