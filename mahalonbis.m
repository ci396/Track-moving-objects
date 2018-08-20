meanvalue=[-0.0847;-0.0201];
meanvalue1=[-0.2679;-0.2279];
XFC=[0.0035 0;0 0.0074];
XFC1=[0.0016 0;0 0.0032];
% AAA=(XFC+XFC1);
% BBB=(meanvalue-meanvalue1)';
Distance=(meanvalue-meanvalue1)'*((XFC+XFC1)^-1)*(meanvalue-meanvalue1);