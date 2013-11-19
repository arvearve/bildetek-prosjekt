'Working....'
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Amount the background can deviate from neutral grey
greyThreshold = 0.1;

% Threshold for distance transfrom. Used to separate close skittles.
distanceThreshold = 0.7;

% Sampling radius for determining average color of each skittle.
samplingRadius = 15;

% Color tolerance used when clustering skittles.
colorDistance = 0.06;

I = double(imread('images\sweetsA01.png'))/255;
J = double(imread('images\sweetsA02.png'))/255;
K = double(imread('images\sweetsA03.png'))/255;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


% First image.
I = double(imread('images\sweetsA01.png'))/255;
[mask, C ] = LocateSkittles(I, 0.1, 0.7, 'images\A01');
result = ClusterByColor(C, mask, 15, 0.06, 'images\A01');
export_output(result, 'A01.txt');

% Second image
[mask, C ] = LocateSkittles(J, 0.1, 0.7, 'images\A02');
result = ClusterByColor(C, mask, 15, 0.06, 'images\A02');
export_output(result, 'A02.txt');

% third image
[mask, C ] = LocateSkittles(K, 0.1, 0.7, 'images\A03');
result = ClusterByColor(C, mask, 15, 0.06, 'images\A03');
export_output(result, 'A03.txt');


'Done!'