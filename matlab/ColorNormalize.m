function [ result ] = ColorNormalize( I )
% Color-normalize an image
%  expects m*n*3 image.
    
    m = size(I, 1);
    n = size(I, 2);
    
    result = zeros(m,n,3);
    for k = 1:m
        for l = 1:n
            colorVector = [I(k,l,1) I(k,l,2) I(k,l,3)];
            normalizedPixel = colorVector / norm(colorVector);
            result(k,l,1) = normalizedPixel(1);
            result(k,l,2) = normalizedPixel(2);
            result(k,l,3) = normalizedPixel(3);
        end
    end
end

