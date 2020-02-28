function [out] = FLCapprox( e1, e2, r1min, r2min, res1, res2, lut, n1, n2)

if(e1<r1min)
    e1 = r1min;
end

if(e2<r2min)
    e2 = r2min;
end

ind1 = round((e1 - r1min) / res1) + 1;

if(ind1 > n1) 
    ind1 = n1;
end

ind2 = round((e2 - r2min) / res2) + 1;

if(ind2 > n2) 
    ind2 = n2;
end

out = double(lut(ind1, ind2)) * 12/255;

end

