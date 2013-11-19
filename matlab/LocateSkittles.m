function [ mask, C ] = LocateSkittles(I, greyTolerance, distanceThresh, output_folder)
% remove shadows and highlights. This leaves us with only color vectors.
C = ColorNormalize(I);
imwrite( C, strcat(output_folder, '\1_NormalizedColors.png'));

%blur to avoid noise errors
h = fspecial('gaussian', [3 3], 2);
Cg = imfilter(C, h, 'symmetric', 'same');

% mask out greys.
R = Cg(:,:,1); 
G = Cg(:,:,2); 
B = Cg(:,:,3);
backgroundMask = (abs(R-G) > greyTolerance) | (abs(R-B) > greyTolerance) | (abs(B-G) > greyTolerance); 
backgroundMask = imfill(backgroundMask, 'holes');
imwrite( backgroundMask, strcat(output_folder, '\2_BackgroundMask.png'));

% distance transform with threshold to break apart connected blobs.
distanceTransform = bwdist(~backgroundMask);
distanceTransform = distanceTransform / max(max(distanceTransform));
imwrite( distanceTransform, strcat(output_folder, '\3_DistanceTransform.png'));

mask = distanceTransform > distanceThresh;
imwrite( mask, strcat(output_folder, '\4_Mask.png'));
imwrite( I + repmat(mask, [1 1 3]), strcat(output_folder, '\5_ImageWithMask.png'));

%grow the mask so  it covers more of the smartie.
mask = bwmorph(mask, 'shrink', Inf);
imwrite( mask, strcat(output_folder, '\6_Points.png'));
end

