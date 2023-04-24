#include "tgaimage.h"
#include "iostream"
#include "model.h"
#include <sstream>
#include <string>
#include "vector"
using  namespace  std;
const double Eplison = 0.001;
const TGAColor white = TGAColor(255, 255, 255, 255);//白色
const TGAColor red   = TGAColor(255, 0,   0,   255);//红色
const TGAColor pink =TGAColor(254,95,85,255);//粉色
const int height=200;
const int width=200;

Model *model=NULL;


void drawline(int x0,int y0,int x1,int y1,TGAImage &image,TGAColor color){
    for (float t=0.0;(1.0-t>Eplison||abs(1.0-t)<Eplison);t=t+0.1)
    {
        float x=x0+(x1-x0)*t;
        float y=y0+(y1-y0)*t;
        image.set(x,y,color);
    }
}
//根据x从x0到x1的变化，按比例计算y
//缺点：计算所得y容易跳变，从而 产生holes
void drawlineV2(int x0,int y0,int x1,int y1,TGAImage &image,TGAColor color)
{
    for(int x=x0;x<=x1;x++)
    {
        float t=(x-x0)/float((x1-x0));
        float y=y0*(1-t)+y1*t;
        image.set(x,y,color);
    }
}

void drawlineV3(int x0,int y0,int x1,int y1,TGAImage &image,TGAColor color)
{
    bool steep=false;
    if (abs(x0-x1)<abs(y0-y1)){
        swap(x0,y0);
        swap(x1,y1);
        steep=true;
    }
    if (x0>x1){
        swap(x0,x1);
        swap(y0,y1);
    }
    for(int x=x0;x<=x1;x++){
        float t=(x-x0)/float((x1-x0));
        float y=y0*(1-t)+y1*t;
        if (steep){
            image.set(y,x,color);
        } else{
            image.set(x,y,color);
        }

    }
}

//第四版：优化除法计算
void drawlineV4(int x0,int y0,int x1,int y1,TGAImage &image,TGAColor color){
    bool steep=false;
    if (abs(x0-x1)<abs(y0-y1)){
        swap(x0,y0);
        swap(x1,y1);
        steep=true;
    }
    if (x0>x1){
        swap(x0,x1);
        swap(y0,y1);
    }
    float derror=abs(y1-y0)/float (abs(x1-x0));
    float  error=0;
    int y=y0;
    for(int x=x0;x<=x1;x++){
        if (steep){
            image.set(y,x,color);
        } else{
            image.set(x,y,color);
        }
        error=error+derror;
        if(error>0.5){
            y=y+(y1>y0?1:-1);
            error=error-1;
        }

    }
}
// TODO:直线绘制算法优化，其实暂时不考虑做^.^
void drawlineV5(int x0,int y0,int x1,int y1,TGAImage &image,TGAColor &color){
    bool steep=false;
    if (abs(x0-x1)<abs(y0-y1)){
        swap(x0,y0);
        swap(x1,y1);
        steep=true;
    }
    if (x0>x1){
        swap(x0,x1);
        swap(y0,y1);
    }

}

//a 0 0
//b 1 1
//c 3 4
//d 5 6
void test_isstringstream(){
    ifstream file;
    file.open("C:\\Users\\Admin\\CLionProjects\\TINY_rendered\\test.txt",ifstream::in);
    if (file.fail()) {
        cout<<"error open"<<endl;
        return;
    }
    string line;
    getline(file,line);

    //读取第一行一个a
    istringstream eachline(line);
    char id;
    eachline>>id;
    cout<<id<<endl;
    char id2;
    eachline>>id2;
    cout<<id2<<endl;

    //读取第二行一个b
    getline(file,line);
    eachline.clear();
    eachline.str(line);
    eachline>>id;
    cout<<id<<endl;


}

//给定三角形坐标，点P，返回点P的坐标系
Vec3f barycentric(Vec2i *pts,Vec2i P){

    //首先计算向量[ABx,ACx,PAx]和[ABy,ACy,PAy]
    Vec3f x(pts[1].x-pts[0].x,pts[2].x-pts[0].x,  pts[0].x-P.x);
    Vec3f y(pts[1].y-pts[0].y,pts[2].y-pts[0].y,pts[0].y-P.y);
    //上述两向量叉乘即为[e,f,g]
    Vec3f  u=x^y;

    //需排除g为0，即ABC三角形退化为线段的情况
    if(abs(u.z)<1){
        return Vec3f (-1,1,1);
    }else{
        //归一化:[u,v,1]=[e/g,f/g,g/g]
        //已知P=(1−u−v)A+uB+vC
        return Vec3f(1.f- (u.x+u.y)/(float)u.z,u.x/(float)u.z,u.y/(float)u.z);
    }
}

//给定三角形三个点坐标，在屏幕上绘制相应三角形
void testTriangles(Vec2i *pts, TGAImage &image, TGAColor color)
{


    //1. 找到三角形边界框的左下角和右上角
    Vec2i bboxmax(0,0);
    Vec2i  bboxmin(width-1,height-1);
    for(int i=0;i<3;i++)
    {
        //这里对边界框的范围进行限制:0到image范围
        bboxmin.x=max(0, min(bboxmin.x,pts[i].x));
        bboxmin.y=max(0,min(bboxmin.y,pts[i].y));
        bboxmax.x=min(width-1, max(bboxmax.x,pts[i].x));
        bboxmax.y=min(height-1,max(bboxmax.x,pts[i].y));

    }

    //2. 列举三角形边界内所有的P，判断是否在三角形内
    Vec2i P;
    for(int i=bboxmin.x;i<=bboxmax.x;i++)
    {
        for(int j=bboxmin.y;j<=bboxmax.y;j++)
        {
            Vec3f save= barycentric(pts,Vec2i(i,j));
            if (save.x<0||save.y<0||save.z<0){
                continue;
            }else{
                image.set(i,j,color);
            }
        }
    }
}



int main(int argc, char** argv) {
    TGAImage image(width,  height, TGAImage::RGB);//100*100的图像，每像素三个字节存储RGB值

   // Wireframe rendering
   //open_obj
   model=new Model("C:\\Users\\Admin\\CLionProjects\\TINY_rendered\\obj\\african_head.obj");
   for(int i=0;i<model->nfaces();i++)//遍历每一个面片
   {
       //获取每个面对应的三个顶点编号
       vector<int> vertexGroup=model->face(i);
       //计算该面的法向

       Vec3f v0=model->vert(vertexGroup[0]);
       Vec3f  v1=model->vert(vertexGroup[1]);
       Vec3f v2=model->vert(vertexGroup[2]);





       //在屏幕上绘制网格

       //视口变换:从[-1 1]变换至[weidth height]
       int x0=(v0.x+1.0)*width/2.;
       int x1=(v1.x+1.0)*width/2.;
       int y0=(v0.y+1.0)*height/2.;
       int y1=(v1.y+1.0)*height/2.;
       drawlineV4(x0,y0,x1,y1,image,white);




   }
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
   image.write_tga_file("output_line4.tga");//写出图像
    delete model;



    return 0;
}