
HitShape shp_st_1d4= hitShape( (-2, 2) );

float patt_st_1d4[ ] = { -1, 2, 0, 2, -1 };

float factor_st_1d4 = 2;


HitShape shp_jacobi2d4 = hitShape( (-1, 1), (-1, 1) );

float patt_jacobi2d4[ ] = {
       0, 1, 0,
       1, 0, 1,
       0, 1, 0
};

float factor_jacobi2d4 = 4;


HitShape shp_jacobi2d9nc = hitShape( (-2, 2), (-2, 2) );

float patt_jacobi2d9nc[ ] = {
       0, 0, 1, 0, 0,
       0, 0, 1, 0, 0,
       1, 1, 1, 1, 1, 
       0, 0, 1, 0, 0,
       0, 0, 1, 0, 0 };

float factor_jacobi2d9nc = 9;


HitShape shp_st_3d27 = hitShape( (-1, 1), (-1, 1), (-1, 1) );

float patt_st_3d27 [] = {
       1, 1, 1,   1, 1, 1,   1, 1, 1,
       1, 1, 1,   1, 1, 1,   1, 1, 1,
       1, 1, 1,   1, 1, 1,   1, 1, 1
};

float factor_st_3d27 = 27;

