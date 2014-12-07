%function for genertaing poisson process
function P=poisson_process(lamda,Tmax)

i=1;T(1)=ceil(random('exponential',lamda));

while(T(i)<Tmax)

    T(i+1)=T(i)+ceil(random('exponential',lamda));

    i=i+1;

end
P=T(1:i-1);

%T(i)=Tmax;x=0:1:i;w(1)=0;

%for p=1:i

 %   w(p+1)=T(p);

%end

%length(w)

%length(x)

%stairs(w,x);
