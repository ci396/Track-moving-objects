clear all
clc
close all

A=imread('D:\FYP\ZILIAO2\clip1\1107.jpg'); 
A(A<250)=0;
A(A>=250)=255;                 % set 2-value image

X1=A;

B=imread('D:\FYP\ZILIAO2\clip3\1107.jpg'); 
B(B<250)=0;
B(B>=250)=255;

X2=B;  %打开图像

H1=[2.5292321e-003 -2.9308219e-003  8.9167899e-001;  % homography matrix
 -1.1296067e-003  1.7427312e-002  4.5228572e-001;
 -4.2612542e-006  2.6108608e-005  5.1213306e-003];
H2=[-7.3366521e-004 -1.1742721e-005  4.7093855e-001;
 -1.9718024e-005 -1.8766241e-003  8.8216347e-001;
  3.9863045e-007  4.5272728e-006  6.5858345e-004];

C=zeros(1050,680);
D=zeros(1050,680);

for i=1:1050
    for j=1:680
        a=(H1^-1)*[j;i;1];
        g=a(1)/a(3);
        f=a(2)/a(3);
%         g=round(g);
%         f=round(f);
        if f<0                  %判断是否在原图中
           D(i,j)=0;continue;
        elseif f>576
           D(i,j)=0;continue;
        elseif g<0
           D(i,j)=0;continue;
        elseif g>720
           D(i,j)=0;continue;
        end
        
%         m1=floor(f);                    %计算距离和色度
%         if m1==0
%             m1=1;
%         end
%         m2=ceil(f);
%         m3=floor(g);
%         if m3==0
%             m3=1;
%         end
%         m4=ceil(g);
        
        D(i,j) = X1(ceil(f),ceil(g));        
%         n1=((m1-f)^2+(m3-g)^2)^0.5;
%         n2=((m2-f)^2+(m3-g)^2)^0.5;
%         n3=((m1-f)^2+(m4-g)^2)^0.5;
%         n4=((m2-f)^2+(m4-g)^2)^0.5;
%        
%         D(i,j)=(X1(m1,m3)*n1+X1(m2,m3)*n2+X1(m1,m4)*n3+X1(m2,m4)*n4)/(n1+n2+n3+n4);
    end
end


subplot(1,2,1)  

imshow(D);

title('图像map1') 

for i=1:1050
    for j=1:680
        b=(H2^-1)*[j;i;1];
        z=b(1)/b(3);
        x=b(2)/b(3);
%         g=round(g);
%         f=round(f);
        if x<0                  %判断是否在原图中
           C(i,j)=0;continue;
        elseif x>576
           C(i,j)=0;continue;
        elseif z<0
           C(i,j)=0;continue;
        elseif z>720
           C(i,j)=0;continue;
        end
        
%         m5=floor(x);                    %计算距离和色度
%         if m5==0
%             m5=1;
%         end
%         m6=ceil(x);
%         m7=floor(z);
%         if m7==0
%             m7=1;
%         end
%         m8=ceil(z);
        
        C(i,j) = X2(ceil(x),ceil(z));
%         n5=((m5-x)^2+(m7-z)^2)^0.5;
%         n6=((m6-x)^2+(m7-z)^2)^0.5;
%         n7=((m5-x)^2+(m8-z)^2)^0.5;
%         n8=((m6-x)^2+(m8-z)^2)^0.5;
%        
%         C(i,j)=(X2(m5,m7)*n5+X2(m6,m7)*n6+X2(m5,m8)*n7+X2(m6,m8)*n8)/(n5+n6+n7+n8);
    end
end
subplot(1,2,2)  

imshow(C);

title('图像map2')  %显示两幅图像

 

% （2）两幅图像直接融合的程序语句

 
E=0.5*C+D/2;
E(E==255)=1;E(E==127.5)=0.5;
figure;subplot(1,2,1)  

imshow(E); %在空域内直接融合

 
title('两图像直接相加融合')  %显示融合后的图像，并命名为“两图像直接相加融合”

F = E;
F(F<1)=0;
subplot(1,2,2)
imshow(F);
BB = bwboundaries(F);
% BB1=cell2mat(BB);
% point=input('please input the number of pixels:');
% AAA1=zeros(point,2);
% for i=1:point
%    bbb=(H1^-1)*[BB1(i,2);BB1(i,1);1];
%    zzz=bbb(1)/bbb(3);
%    xxx=bbb(2)/bbb(3);
%    AAA1(i,1)=xxx;
%    AAA1(i,2)=zzz;
% end
% [CCC III]=max(AAA1);
% [DDD EEE]=min(AAA1);
% CCC=round(CCC);
% DDD=round(DDD);
