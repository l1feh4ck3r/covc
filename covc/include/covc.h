#ifndef VC_H
#define VC_H

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C"
{
#endif

void    DLL_EXPORT  add_image_and_matrix(image, matrix);
void    DLL_EXPORT  add_image_and_matrix_and_boundingrectangle(image, matrix, boundingrectangle);
void    DLL_EXPORT  add_camera_clibration_matrix(matrix);
void    DLL_EXPORT  set_result_cube_resolution(int, int, int);
void *  DLL_EXPORT  get_voxel_model();
void    DLL_EXPORT  clear();

#ifdef __cplusplus
}
#endif

#endif // VC_H
