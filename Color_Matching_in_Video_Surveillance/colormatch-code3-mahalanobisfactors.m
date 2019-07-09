%%%%%%%%%%%%clauster 1%%%%%%%%%%%%
ZZ1=find(Idx==1);                        % all points of cluster 1
[cr1,cc1]=size(ZZ1);        
vax1=0;
vay1=0;
for i=1:cr1
vax1=vax1+(X(1,ZZ1(i,1))-C(1,1))^2;          % (x-ux)^2
vay1=vay1+(Y(1,ZZ1(i,1))-C(1,2))^2;          % (y-uy)^2
end
vax1=vax1/cr1;
vay1=vay1/cr1;
XFC1=[vax1,0;0,vay1];                         %covariance matrix
%%%%%%%%%%%%%%clauster 2%%%%%%%%%%
ZZ2=find(Idx==2);
[cr2,cc2]=size(ZZ2);
vax2=0;
vay2=0;
for i=1:cr2
  vax2=vax2+(X(1,ZZ2(i,1))-C(2,1))^2;
  vay2=vay2+(X(1,ZZ2(i,1))-C(2,2))^2;
end
vax2=vax2/cr2;
vay2=vay2/cr2;
XFC2=[vax2,0;0,vay2];
%%%%%%%%%%%%%%%clauster 3%%%%%%%%%%%%%
ZZ3=find(Idx==3);
[cr3,cc3]=size(ZZ3);
vax3=0;
vay3=0;
for i=1:cr3
   vax3=vax3+(X(1,ZZ3(i,1))-C(3,1))^2;
   vay3=vay3+(X(1,ZZ3(i,1))-C(3,2))^2;
end
vax3=vax3/cr3;
vay3=vay3/cr3;
XFC3=[vax3,0;0,vay3];
