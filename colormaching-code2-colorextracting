 a=importdata('F:/FYP/ZILIAO1/point3/865.txt');
img=imread('F:/FYP/ZILIAO1/clip3\0865.jpg');
img1=imread('F:/FYP/ZILIAO2/clip3/865.jpg');
i=input('please input:');
ai1=0.8*a(i,1)+0.2*a(i,2);
ai2=0.55*a(i,1)+0.45*a(i,2);
imgchange=img(ai1:ai2,a(i,3):a(i,4),:);
imgchange1=img1(ai1:ai2,a(i,3):a(i,4),:);
subplot(121)
imshow(imgchange);
subplot(122)
imshow(imgchange1);
[m,n,dim]=size(imgchange);
p=uint8(zeros(m,n,3)) ;
bb=0;
  for ix=1:m
      for jx=1:n          
          if imgchange1(ix,jx,1)>=10             
              bb=bb+1;                       
          end
      end
  end
 
new=uint8(zeros(b,3));  
ib=1;

  for ix=1:m
      for jx=1:n 
        
              if imgchange1(ix,jx,1)>=10
             new(ibb,1)=imgchange(ix,jx,1);
             new(ibb,2)=imgchange(ix,jx,2);
             new(ibb,3)=imgchange(ix,jx,3);
             ibb=ibb+1;
             
               
          end
      end
  end
 

new=reshape(new,1,bb,3);   %聚类的样本
HSV=rgb2hsv(new);
H=HSV(:,:,1);
S=HSV(:,:,2);
V=HSV(:,:,3);

J=H*2*pi;
X=zeros(1,bb);
Y=zeros(1,bb);
  [X,Y]=pol2cart(J,S);

figure;
plot(X,Y,'b.'); 
axis([-1.5 1.5 -1.5 1.5]);
hold on
grid on
title('H & S distribution of the picture ');

%% %%%%%%%%%%%%%%%%%%%%%%%% 聚 类 %%%%%%%%%%%%%%%%%%%%%%%%%
HS=zeros(b,2);
HS(:,1)=X;
HS(:,2)=Y;
K=3;
[Idx,C,sumD,D]=kmeans(HS,K,'distance','sqEuclidean','Replicates',10);

%%          换顺序  %%%%%%%%%%%%%%%%%%%%%
dis1=C(1,1)^2+C(1,2)^2;
dis2=C(2,1)^2+C(2,2)^2;
dis3=C(3,1)^2+C(3,2)^2;

if (dis1<=dis2) && (dis2>=dis3) &&(dis3>=dis1)
    
    TT=C(2,:);
    C(2,:)=C(3,:);
    C(3,:)=TT;
    
    for x=1:bb
        if Idx(x)==2
            Idx(x)=3;
        elseif  Idx(x)==3
            Idx(x)=2;
        end
    end
    
elseif (dis1>=dis2) && (dis3>=dis2) && (dis3>=dis1)
    
    TT=C(2,:);
    C(2,:)=C(1,:);
    C(1,:)=TT;
    
             for x=1:bb
                if Idx(x)==2
                    Idx(x)=1;
                elseif  Idx(x)==1
                    Idx(x)=2;
                end
             end
elseif (dis1<=dis2) && (dis2>=dis3) && (dis3<=dis1)
        TT=C(1,:);
        C(1,:)=C(3,:);
        C(3,:)=TT;
        TT1=C(2,:);
        C(2,:)=C(3,:);
        C(3,:)=TT1;
        for x=1:bb
                if Idx(x)==2
                    Idx(x)=3;
                elseif  Idx(x)==3
                    Idx(x)=1;
                else Idx(x)=2;
                    
                end
        end
             
    elseif (dis1>=dis2) && (dis2<=dis3) && (dis3<=dis1)
        TT=C(1,:);
        C(1,:)=C(2,:);
        C(2,:)=TT;
        
        TT1=C(2,:);
        C(2,:)=C(3,:);
        C(3,:)=TT1;
        for x=1:bb
                if Idx(x)==2
                    Idx(x)=1;
                elseif  Idx(x)==1
                    Idx(x)=3;
                else Idx(x)=2;                    
                end
             end
elseif (dis1>=dis2) && (dis2>=dis3) && (dis3<=dis1)
        TT=C(1,:);
        C(1,:)=C(3,:);
        C(3,:)=TT;
        
        
        for x=1:bb
                if Idx(x)==3
                    Idx(x)=1;
                elseif  Idx(x)==1
                    Idx(x)=3;
                                 
                end
        end
end

%% %%%%%%%%%%%%%%%%%%%% 画聚类后的散点图 %%%%%%%%%%%%%%%%%%%


plot(HS(Idx==1,1),HS(Idx==1,2),'r.')
axis([-1.5 1.5 -1.5 1.5]);
hold on 

plot(HS(Idx==2,1),HS(Idx==2,2),'b.')
hold on 

plot(HS(Idx==3,1),HS(Idx==3,2),'g.') 

plot(C(:,1),C(:,2),'kx','MarkerSize',14,'LineWidth',4)  

title('H & S original distribution ');

legend('Cluster 1','Cluster 2','Cluster 3','Centroids','Location','NW')
%%  画两个圆：
alpha=0:pi/20:2*pi; 
R=1; 
x=R*cos(alpha);
y=R*sin(alpha);
plot(x,y,'-')
R=0.3; 
x=R*cos(alpha);
y=R*sin(alpha);
plot(x,y,'-')
axis([-1.5 1.5 -1.5 1.5]);

%%  转回极坐标 表示 
[J1,S1]=cart2pol(C(1,1),C(1,2));
 
 if J1>=0
     H1=J1/(2*pi);
 else
     H1=(2*pi+J1)/(2*pi);
 end
 
 [J2,S2]=cart2pol(C(2,1),C(2,2));
   if J2>=0
     H2=J2/(2*pi);
 
 else H2=(2*pi+J2)/(2*pi);
  end 
 
 [J3,S3]=cart2pol(C(3,1),C(3,2));
     if J3>=0
     H3=J3/(2*pi);
 
 else H3=(2*pi+J3)/(2*pi);
     end 
     
ang1=abs(H1-H2);
ang2=abs(H3-H2);
ang3=abs(H1-H3);

          
if  S1<=0.3 && S2>=0.3            %%%%%center1 in black-grey-white
     
   
 
 


 
 
      if ang2<=0.167||ang2>=0.833
          
    
         nX=mean(HS(Idx==3|Idx==2,1));
         nY=mean(HS(Idx==3|Idx==2,2));
         [J,nS]=cart2pol(nX,nY);
             if J>=0
                 nH=J/(2*pi);
             else
                 nH=(2*pi+J)/(2*pi);
             end           
 
figure;
plot(HS(Idx==3,1),HS(Idx==3,2),'r.')
axis([-1.5 1.5 -1.5 1.5]);
grid on
hold on 
plot(HS(Idx==2,1),HS(Idx==2,2),'r.')
hold on 
% plot(HS(Idx==1,1),HS(Idx==1,2),'g.')
% hold on
% plot(C(1,1),C(1,2),'kx','MarkerSize',14,'LineWidth',4);  
% hold on
plot(nX,nY,'kx','MarkerSize',14,'LineWidth',4);  

title('H & S final distribution ');
legend('Cluster ','Centroids','Location','NW')


Vmean=mean(V(Idx==1));
gray=ones(50,50,3);
gray1(:,:,1)=gray(:,:,1)*H1;
gray1(:,:,2)=gray(:,:,2)*S1;
gray1(:,:,3)=gray(:,:,3)*Vmean;


 figure;  
 c1=hsv2rgb(gray1);
 subplot(211);
 imshow(c1); 
 s1=sum((Idx==1))/b;
 title(s1*100);

col=ones(50,50,3);
s2=sum((Idx==3|Idx==2))/b;
 nV=mean(V(Idx==3|Idx==2));
col1(:,:,1)=col(:,:,1)*nH;
col1(:,:,2)=col(:,:,2)*nS;
col1(:,:,3)=col(:,:,3)*nV;
  
 n1=hsv2rgb(col1);
 subplot(212);
 imshow(n1);
 title(s2*100);
 %%   
      else
          
          
s1=sum((Idx==1))/b;
s2=sum((Idx==2))/b; 
s3=sum((Idx==3))/b;

 
 Vmean=mean(V(Idx==1));
gray=ones(50,50,3);
gray1(:,:,1)=gray(:,:,1)*H1;
gray1(:,:,2)=gray(:,:,2)*S1;
gray1(:,:,3)=gray(:,:,3)*Vmean;


 figure;  
 c1=hsv2rgb(gray1);
 subplot(311);
 imshow(c1); 
 title(s1*100);

col=ones(50,50,3);

V2=mean(V(Idx==2));
col2(:,:,1)=col(:,:,1)*H2;
col2(:,:,2)=col(:,:,2)*S2;
col2(:,:,3)=col(:,:,3)*V2;

 n2=hsv2rgb(col2);
 subplot(312);
 imshow(n2);
  title(s2*100);
  
  
 V3=mean(V(Idx==3));
col3(:,:,1)=col(:,:,1)*H3;
col3(:,:,2)=col(:,:,2)*S3;
col3(:,:,3)=col(:,:,3)*V3;

  
 n3=hsv2rgb(col3);
 subplot(313);
 imshow(n3);
 title(s3*100);
      end          
 
 
 
elseif S1<=0.3 && S2<=0.3 && S3>=0.3                                              %% cluste 1,2 belong to black-grey-white
              
s1=sum((Idx==1))/b;
s2=sum((Idx==2))/b; 
s3=sum((Idx==3))/b;
       nH=mean(H(Idx==1|Idx==2));
       nS=mean(S(Idx==1|Idx==2));
    Vmean=mean(V(Idx==1|Idx==2));
    gray=ones(50,50,3);
gray1(:,:,1)=gray(:,:,1)*0.1;
gray1(:,:,2)=gray(:,:,2)*0.1;
gray1(:,:,3)=gray(:,:,3)*Vmean;

 figure;  
 c1=hsv2rgb(gray1);
 imshow(c1);
 ss=s1+s2;
 
 title(ss*100);
 
  V3=mean(V(Idx==3));
    col=ones(50,50,3);
col2(:,:,1)=col(:,:,1)*H3;
col2(:,:,2)=col(:,:,2)*S3;
col2(:,:,3)=col(:,:,3)*V3;

 figure;  
 c2=hsv2rgb(col2);
 imshow(c2);
 
 title(s3*100); 
 
elseif S1<=0.3 && S2<=0.3 && S3<=0.3
    
              
 

    Vmean=mean(V);
    gray=ones(50,50,3);
gray1(:,:,1)=gray(:,:,1)*0.1;
gray1(:,:,2)=gray(:,:,2)*0.1;
gray1(:,:,3)=gray(:,:,3)*Vmean;

 figure;  
 c1=hsv2rgb(gray1);
 imshow(c1);
 title('the figure is in gray ');
 

   
elseif S1>=0.3     
    if (ang1<=0.167||ang1>=0.833)&&(ang2<=0.167||ang2>=0.833)&&(ang3<=0.167||ang3>=0.833)        %%  3 clusters combined into 1 cluster
      
                nX=mean(X);
                nY=mean(Y);
                 nV=mean(V);

                [J,nS]=cart2pol(nX,nY);

             if J>=0
                 nH=J/(2*pi);
             else
                 nH=(2*pi+J)/(2*pi);
             end
             
figure;             
plot(X,Y,'b.');
hold on 
grid on
plot(nX,nY,'kx','MarkerSize',14,'LineWidth',4);  
axis([-1.5 1.5 -1.5 1.5]);
hold on 
 
col=ones(50,50,3);
col1(:,:,1)=col(:,:,1)*nH;
col1(:,:,2)=col(:,:,2)*nS;
col1(:,:,3)=col(:,:,3)*nV;

 figure;  
 n1=hsv2rgb(col1);
 imshow(n1);
 title('only one type col ');

  

%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% case 2  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
elseif (ang1<=0.167||ang1>=0.833)&&(ang2<=0.833&&ang2>=0.167)&&(ang3<=0.833&&ang3>=0.167)  %cluster 1 and 2 alike  
    
         nX=mean(HS(Idx==1|Idx==2,1));
         nY=mean(HS(Idx==1|Idx==2,2));
         nV=mean(V(Idx==1|Idx==2));
         [J,nS]=cart2pol(nX,nY);

             if J>=0
                 nH=J/(2*pi);
             else
                 nH=(2*pi+J)/(2*pi);
             end
             
 
figure;

plot(HS(Idx==1,1),HS(Idx==1,2),'r.')
axis([-1.5 1.5 -1.5 1.5]);
grid on
hold on 
plot(HS(Idx==2,1),HS(Idx==2,2),'r.')
hold on 
plot(HS(Idx==3,1),HS(Idx==3,2),'g.')
hold on
plot(C(3,1),C(3,2),'kx','MarkerSize',14,'LineWidth',4);  
hold on
plot(nX,nY,'kx','MarkerSize',14,'LineWidth',4);  

title('H & S final distribution ');
legend('Cluster 1','Cluster 2','Centroids','Location','NW')

%%
col=ones(50,50,3);
s1=sum((Idx==1|Idx==2))/b;
s2=sum((Idx==3))/b;

col1(:,:,1)=col(:,:,1)*nH;
col1(:,:,2)=col(:,:,2)*nS;
col1(:,:,3)=col(:,:,3)*nV;

 figure;  
 n1=hsv2rgb(col1);
 subplot(211);
 imshow(n1);
 title(s1*100);
V3=mean(V(Idx==3)); 
col2(:,:,1)=col(:,:,1)*H3;
col2(:,:,2)=col(:,:,2)*S3;
col2(:,:,3)=col(:,:,3)*V3;


 n2=hsv2rgb(col2);
 subplot(212);
 imshow(n2);
  title(s2*100);
%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
elseif (ang2<=0.167||ang2>=0.833)&&(ang1<=0.833&&ang1>=0.167)&&(ang3<=0.833&&ang3>=0.167)
    
         nX=mean(HS(Idx==3|Idx==2,1));
         nY=mean(HS(Idx==3|Idx==2,2));
         nV=mean(V(Idx==3|Idx==2));
         [J,nS]=cart2pol(nX,nY);
             if J>=0
                 nH=J/(2*pi);
             else
                 nH=(2*pi+J)/(2*pi);
             end           
 
figure;
plot(HS(Idx==3,1),HS(Idx==3,2),'r.')
axis([-1.5 1.5 -1.5 1.5]);
grid on
hold on 
plot(HS(Idx==2,1),HS(Idx==2,2),'r.')
hold on 
plot(HS(Idx==1,1),HS(Idx==1,2),'g.')
hold on
plot(C(1,1),C(1,2),'kx','MarkerSize',14,'LineWidth',4);  
hold on
plot(nX,nY,'kx','MarkerSize',14,'LineWidth',4);  

title('H & S final distribution');
legend('Cluster 1','Cluster 2','Centroids','Location','NW')
%%
col=ones(50,50,3);
s1=sum((Idx==3|Idx==2))/b;
s2=sum((Idx==1))/b;

col1(:,:,1)=col(:,:,1)*nH;
col1(:,:,2)=col(:,:,2)*nS;
col1(:,:,3)=col(:,:,3)*nV;

 figure;  
 n1=hsv2rgb(col1);
 subplot(211);
 imshow(n1);
 title(s1*100);
 
 V1=mean(V(Idx==1));
col2(:,:,1)=col(:,:,1)*H1;
col2(:,:,2)=col(:,:,2)*S1;
col2(:,:,3)=col(:,:,3)*V1;


 n2=hsv2rgb(col2);
 subplot(212);
 imshow(n2);
  title(s2*100);
%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
elseif (ang3<=0.167||ang3>=0.833)&&(ang1<=0.833&&ang1>=0.167)&&(ang2<=0.833&&ang2>=0.167)
    
         nX=mean(HS(Idx==3|Idx==1,1));
         nY=mean(HS(Idx==3|Idx==1,2));
         nV=mean(V(Idx==3|Idx==1))
         
         [J,nS]=cart2pol(nX,nY);
             if J>=0
                 nH=J/(2*pi);
             else
                 nH=(2*pi+J)/(2*pi);
             end           
 
figure;
plot(HS(Idx==3,1),HS(Idx==3,2),'g.')
axis([-1.5 1.5 -1.5 1.5]);
grid on
hold on 
plot(HS(Idx==2,1),HS(Idx==2,2),'r.')
hold on 
plot(HS(Idx==1,1),HS(Idx==1,2),'g.')
hold on
plot(C(2,1),C(2,2),'kx','MarkerSize',14,'LineWidth',4);  
hold on
plot(nX,nY,'kx','MarkerSize',14,'LineWidth',4);  

title('H & S final distribution ');
legend('Cluster 1','Cluster 2','Centroids','Location','NW')
        
%%
col=ones(50,50,3);
s1=sum((Idx==3|Idx==1))/b;
s2=sum((Idx==2))/b;

col1(:,:,1)=col(:,:,1)*nH;
col1(:,:,2)=col(:,:,2)*nS;
col1(:,:,3)=col(:,:,3)*nV;

 figure;  
 n1=hsv2rgb(col1);
 subplot(211);
 imshow(n1);
 title(s1*100);
 
 V2=mean(V(Idx==2));
col2(:,:,1)=col(:,:,1)*H2;
col2(:,:,2)=col(:,:,2)*S2;
col2(:,:,3)=col(:,:,3)*V2;


 n2=hsv2rgb(col2);
 subplot(212);
 imshow(n2);
  title(s2*100);    
 

else


%%
     
 
s1=sum((Idx==1))/b;
s2=sum((Idx==2))/b; 
s3=sum((Idx==3))/b;
V1=mean(V(Idx==1));
V2=mean(V(Idx==2));
V3=mean(V(Idx==2));

 


col=ones(50,50,3);

col1(:,:,1)=col(:,:,1)*H1;
col1(:,:,2)=col(:,:,2)*S1;
col1(:,:,3)=col(:,:,3)*V1;

  figure;  
 n1=hsv2rgb(col1);
 subplot(311);
 imshow(n1);
 title(s1*100);
 
col2(:,:,1)=col(:,:,1)*H2;
col2(:,:,2)=col(:,:,2)*S2;
col2(:,:,3)=col(:,:,3)*V2;


 n2=hsv2rgb(col2);
 subplot(312);
 imshow(n2);
  title(s2*100);
 
col3(:,:,1)=col(:,:,1)*H3;
col3(:,:,2)=col(:,:,2)*S3;
col3(:,:,3)=col(:,:,3)*V3;

  
 n3=hsv2rgb(col3);
 subplot(313);
 imshow(n3);
 title(s3*100);
    end
end
% %***********************************************************************
 
