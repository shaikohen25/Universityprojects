%Q2d
clear all;clc;close all;
Rg=50;
l=0.1;
L0=5*10^-7;
C0=8.8889*10^-11;
ZL=150;
Z0=sqrt(L0/C0);
N= input('Enter number ');
d=l/(N);
f_new=linspace(0.88*10^10 ,1.115*10^10);
y=2.*pi.*f_new;
L1=106/(2.85714286*10^8);
C1=1/(2.85714286*106*10^8);
V_maxima=zeros(100,1);
V_minima=zeros(100,1);
a1=0;
a2=0;
n_0=round(N/2)-1;
n_1=N-n_0+1;
L0_array=L0*ones(1,n_0);
C0_array=C0*ones(1,n_0);
z=linspace(l/2,l,n_1 + 6);
s=1;
for i =1:100;
     w=y(i) ;
     L1_array= L1 + a1.*(z-l./2).^2;
    C1_array= C1 + a2.*(z-l./2).^2;
    L=[L0_array L1_array];
    C=[C0_array C1_array];
    A1=(zeros(N-1,2*N+2));
    for i= 1:N-1
          A1(i,i)=1;
          A1(i,i+2)=-1;
          A1(i,N+2+i)=-j*2*w*d*L(i);
     end
     A2=(zeros(N-1,2*N+2));
     for i= 1:N-1
          A2(i,i+1)=-j*2*w*C(i)*d;
          A2(i,N+1+i)=1;
           A2(i,N+i+3)=-1;
     end
     A3=(zeros(4,2*N+2));
     A3(1,1)=1; A3(1,N+2)=Rg;
     A3(2,N+1)=1; A3(2,2*N+2)=-ZL;
     A3(3,1)=-1; A3(3,2)=1; A3(3,N+2)=j*w*L(1)*d/2; A3(3,N+3)=j*w*L(1)*d/2;
     A3(4,N)=-1; A3(4,N+1)=1; A3(4,2*N+2)=j*w*L(N+1)*d/2; A3(4,2*N+1)=j*w*L(N+1)*d/2;
     A=[A1;A2;A3];
     b=zeros(2*N+2,1);
     b(2*N-1)=1;
     x= linsolve(A,b);
     voltage=zeros(N+1,1);
     for i=1:N+1
         voltage(i)=x(i);
     end
     V_maxima(s)=max(abs(voltage(1:n_0)));
     V_minima(s)=min(abs(voltage(1:n_0)));
     s=s+1;
end
array_sum=V_maxima + V_minima;
array_diff=V_maxima - V_minima;
gamma_new= array_diff./array_sum;
figure();
plot(f_new,abs(gamma_new));
title(' Absolute value of the reflection coefficient');



