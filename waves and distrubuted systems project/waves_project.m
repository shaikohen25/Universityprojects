clear all; close all;
% given constants%
eo=8.85*10^-12;
uo=4*pi*10^-7;
f=10^10;
vp=1.5*10^8;
l=0.1;
Zl=150; % load impedance%
Z0=75; % characteristic impedance%
Rg=50; % generator resistance
% calculating constants
w=2*pi*f;
L=75/vp;
C=1/(75*vp);
% now constructing body of the code%
N=input('Enter number ');
d=l/N; % Number of sections of TL%
A1=zeros(N-1,2*N+2);
for i= 1:N-1
    A1(i,i)=1;
    A1(i,i+2)=-1;
    A1(i,N+2+i)=-j*2*w*d*L;
end
A2=zeros(N-1,2*N+2);
for i= 1:N-1
    A2(i,i+1)=-j*2*w*C*d;
    A2(i,N+1+i)=1;
    A2(i,N+i+3)=-1;
end
% Now the matrix of the 4 boundary conditions%
A3=zeros(4,2*N+2);
A3(1,1)=1; A3(1,N+2)=Rg;
A3(2,N+1)=1; A3(2,2*N+2)=-Zl;
A3(3,1)=-1; A3(3,2)=1; A3(3,N+2)=j*w*L*d/2; A3(3,N+3)=j*w*L*d/2;
A3(4,N)=-1; A3(4,N+1)=1; A3(4,2*N+2)=j*w*L*d/2; A3(4,2*N+1)=j*w*L*d/2;
% Now concatenate all the matrix to get the matrix of the system%
A=[A1;A2;A3];
b=zeros(2*N+2,1);
b(2*N-1)=1;
x= linsolve(A,b);
v=zeros(N+1,1);
for i= 1:N+1
    v(i,1)=abs(x(i,1));
end
figure;
plot(1:N+1,vpa(v.'))
title('The numerical solution of the Voltage across the TL');
k=zeros(N+1,1);
for j=1:N+1
    k(j,1)=abs(x(N+1+j,1));
end
figure;
plot(1:N+1,vpa(k.'));
title(' The numerical solution of the Current across the TL');
% Now we try to plot the analytical solution
Gl=(Zl-Z0)/(Zl+Z0);
Zin=Z0*(1+Gl*exp(1i*-2*w*sqrt(L*C)*l))/(1-Gl*exp(1i*-2*w*sqrt(L*C)*l));
V_plus_0=(Zin/(Zin+Rg))*(1/(1+Gl*exp(1i*-2*w*sqrt(L*C)*l)));
x=0:0.0001:0.1;
y= V_plus_0*(1+Gl*exp(1i*2*w*sqrt(L*C)*(x-l)));
figure;
plot(x,abs(y));
title('The analytical solution of the voltage across the TL');
y1=V_plus_0*(1-Gl*exp(1i*2*w*sqrt(L*C)*(x-l)))/Z0;
figure;
plot(x,abs(y1));
title('The analytical solution of current along the TL')




