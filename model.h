#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

class Model {
private:
	std::vector<Vec3f> verts_; //顶点集，每个顶点都是三维向量
	std::vector<std::vector<int> > faces_; //面片集
public:
    Model(const char *filename);//根据.obj文件路径导入模型
    ~Model();
    int nverts();//返回模型顶点数量
    int nfaces();//返回模型面片数量
    Vec3f vert(int i);//返回第i个顶点
    std::vector<int> face(int idx);//返回第idx个面片
};

#endif //__MODEL_H__
