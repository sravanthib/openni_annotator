/****************************************************************************
*                                                                           *
*   Nite 1.3 - Players                                                      *
*                                                                           *
*   Author:     Oz Magal                                                    *
*                                                                           *
****************************************************************************/

/****************************************************************************
*                                                                           *
*   Nite 1.3	                                                            *
*   Copyright (C) 2006 PrimeSense Ltd. All Rights Reserved.                 *
*                                                                           *
*   This file has been provided pursuant to a License Agreement containing  *
*   restrictions on its use. This data contains valuable trade secrets      *
*   and proprietary information of PrimeSense Ltd. and is protected by law. *
*                                                                           *
****************************************************************************/

#include "SceneDrawer.h"
#include <sys/time.h>

#ifdef USE_GLUT
#include <GL/glut.h>
#else
#include "opengles.h"
#endif

extern xn::UserGenerator g_UserGenerator;
extern xn::DepthGenerator g_DepthGenerator;
extern xn::ImageGenerator g_ImageGenerator;
extern double imageNum;
//extern time_t prevNum;

#define MAX_DEPTH 10000
#define OUTPUTDIR "/home/sravanthi/projects/openniprac/rgb_niteImgs"
#define DOUTPUTDIR "/home/sravanthi/projects/openniprac/depth_niteImgs"
float g_pDepthHist[MAX_DEPTH];

double get_time()
{
    struct timeval cur;
    gettimeofday(&cur, NULL);
    return cur.tv_sec + ((cur.tv_usec  / 1000) * 0.0001) ;
}

unsigned int getClosestPowerOfTwo(unsigned int n)
{
	unsigned int m = 2;
	while(m < n) m<<=1;

	return m;
}
GLuint initTexture(void** buf, int& width, int& height)
{
	GLuint texID = 0;
	glGenTextures(1,&texID);

	width = getClosestPowerOfTwo(width);
	height = getClosestPowerOfTwo(height); 
	*buf = new unsigned char[width*height*4];
	glBindTexture(GL_TEXTURE_2D,texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return texID;
}

GLfloat texcoords[8];
void DrawRectangle(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY)
{
	GLfloat verts[8] = {	topLeftX, topLeftY,
		topLeftX, bottomRightY,
		bottomRightX, bottomRightY,
		bottomRightX, topLeftY
	};
	glVertexPointer(2, GL_FLOAT, 0, verts);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	//TODO: Maybe glFinish needed here instead - if there's some bad graphics crap
	glFlush();
}
void DrawTexture(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY)
{
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoords);

	DrawRectangle(topLeftX, topLeftY, bottomRightX, bottomRightY);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

XnFloat Colors[][3] =
{
	{0,1,1},
	{0,0,1},
	{0,1,0},
	{1,1,0},
	{1,0,0},
	{1,.5,0},
	{.5,1,0},
	{0,.5,1},
	{.5,0,1},
	{1,1,.5},
	{1,1,1}
};
XnUInt32 nColors = 10;

void glPrintString(void *font, char *str)
{
	int i,l = strlen(str);

	for(i=0; i<l; i++)
	{
		glutBitmapCharacter(font,*str++);
	}
}

int DrawLimb(XnUserID player, XnSkeletonJoint eJoint1, XnSkeletonJoint eJoint2)
{
	if (!g_UserGenerator.GetSkeletonCap().IsCalibrated(player))
	{
		printf("not calibrated!\n");
		return 0;
	}
	if (!g_UserGenerator.GetSkeletonCap().IsTracking(player))
	{
		printf("not tracked!\n");
		return 0;
	}

	
	XnSkeletonJointPosition joint1, joint2;
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJoint1, joint1);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJoint2, joint2);

	if (joint1.fConfidence < 0.5 || joint2.fConfidence < 0.5)
	{
		return 0;
	}

	XnPoint3D pt[2];
	pt[0] = joint1.position;
	pt[1] = joint2.position;
        
	g_DepthGenerator.ConvertRealWorldToProjective(2, pt, pt);
//	if(prevNum!=imageNum) { 
	printf("%.3f %d Position:%f %f %f \n",imageNum, eJoint1, pt[0].X, pt[0].Y, pt[0].Z);
	printf("%.3f %d Position:%f %f %f \n",imageNum, eJoint2, pt[1].X, pt[1].Y, pt[1].Z);	
//	}
	glVertex3i(pt[0].X, pt[0].Y, 0);
	glVertex3i(pt[1].X, pt[1].Y, 0);
	return 1;
}


void dump_depth(FILE *fp, void *data, int width, int height, int data_size)
{
    fprintf(fp, "P5 %d %d 65535\n", width, height);
    fwrite(data, data_size, width*height, fp);
}


void saveDepthImage(const xn::DepthMetaData &dmd) {
	cv::Mat depthArr;
	char outputImg[1000];
	XnUInt16 g_nXRes = dmd.XRes();
	XnUInt16 g_nYRes = dmd.YRes();
	unsigned int nValue = 0;

	sprintf(outputImg, "%s/%.3f.pgm",DOUTPUTDIR,imageNum);

    FILE* fp = fopen(outputImg, "w");
	if(!fp) { 
		printf("Cannot open file %s\n",outputImg);
		exit(1);
    }
	// 2-byte channel for unsigned short
	dump_depth(fp, (unsigned short *)dmd.Data(), dmd.XRes(), dmd.YRes(), 2);
	fclose(fp);
}

void saveImage(const xn::ImageMetaData &imd) { 
	cv::Mat colorArr[3];
        cv::Mat colorImage;
	char outputImg[1000];
        const XnRGB24Pixel* pImageRow;

	sprintf(outputImg,"%s/%.3f.jpg",OUTPUTDIR,imageNum);
        pImageRow = imd.RGB24Data();

        colorArr[0] = cv::Mat(imd.YRes(), imd.XRes(), CV_8U);
        colorArr[1] = cv::Mat(imd.YRes(), imd.XRes(), CV_8U);
        colorArr[2] = cv::Mat(imd.YRes(), imd.XRes(), CV_8U);

        for(int y=0; y<imd.YRes(); y++) {
                const XnRGB24Pixel *pPixel = pImageRow;
                uchar *Bptr = colorArr[0].ptr<uchar>(y);
                uchar *Gptr = colorArr[1].ptr<uchar>(y);
                uchar *Rptr = colorArr[2].ptr<uchar>(y);
                for(int x=0;x<imd.XRes();x++,++pPixel) {
                        Bptr[x] = pPixel->nBlue;
                        Gptr[x] = pPixel->nGreen;
                        Rptr[x] = pPixel->nRed;
                }
                pImageRow += imd.XRes();
        }
        cv::merge(colorArr,3,colorImage);
	
        IplImage img=colorImage;
        cvSaveImage(outputImg,&img);
}


void DrawDepthMap(const xn::DepthMetaData& dmd, const xn::ImageMetaData& imd, const xn::SceneMetaData& smd, XnUserID player)
{
	static bool bInitialized = false;	
	static GLuint depthTexID;
	static unsigned char* pDepthTexBuf;
	static int texWidth, texHeight;

	float topLeftX;
	float topLeftY;
	float bottomRightY;
	float bottomRightX;
	float texXpos;
	float texYpos;

	if(!bInitialized)
	{

		texWidth =  getClosestPowerOfTwo(dmd.XRes());
		texHeight = getClosestPowerOfTwo(dmd.YRes());

//		printf("Initializing depth texture: width = %d, height = %d\n", texWidth, texHeight);
		depthTexID = initTexture((void**)&pDepthTexBuf,texWidth, texHeight) ;

//		printf("Initialized depth texture: width = %d, height = %d\n", texWidth, texHeight);
		bInitialized = true;

		topLeftX = dmd.XRes();
		topLeftY = 0;
		bottomRightY = dmd.YRes();
		bottomRightX = 0;
		texXpos =(float)dmd.XRes()/texWidth;
		texYpos  =(float)dmd.YRes()/texHeight;

		memset(texcoords, 0, 8*sizeof(float));
		texcoords[0] = texXpos, texcoords[1] = texYpos, texcoords[2] = texXpos, texcoords[7] = texYpos;

	}
	unsigned int nValue = 0;
	unsigned int nHistValue = 0;
	unsigned int nIndex = 0;
	unsigned int nX = 0;
	unsigned int nY = 0;
	unsigned int nNumberOfPoints = 0;
	XnUInt16 g_nXRes = dmd.XRes();
	XnUInt16 g_nYRes = dmd.YRes();

	unsigned char* pDestImage = pDepthTexBuf;

	const XnDepthPixel* pDepth = dmd.Data();
	const XnLabel* pLabels = smd.Data();

	// Calculate the accumulative histogram
	memset(g_pDepthHist, 0, MAX_DEPTH*sizeof(float));
	for (nY=0; nY<g_nYRes; nY++)
	{
		for (nX=0; nX<g_nXRes; nX++)
		{
			nValue = *pDepth;

			if (nValue != 0)
			{
				g_pDepthHist[nValue]++;
				nNumberOfPoints++;
			}

			pDepth++;
		}
	}

	for (nIndex=1; nIndex<MAX_DEPTH; nIndex++)
	{
		g_pDepthHist[nIndex] += g_pDepthHist[nIndex-1];
	}
	if (nNumberOfPoints)
	{
		for (nIndex=1; nIndex<MAX_DEPTH; nIndex++)
		{
			g_pDepthHist[nIndex] = (unsigned int)(256 * (1.0f - (g_pDepthHist[nIndex] / nNumberOfPoints)));
		}
	}

	pDepth = dmd.Data();
	{
		XnUInt32 nIndex = 0;
		// Prepare the texture map
		for (nY=0; nY<g_nYRes; nY++)
		{
			for (nX=0; nX < g_nXRes; nX++, nIndex++)
			{
				nValue = *pDepth;
				XnLabel label = *pLabels;
				XnUInt32 nColorID = label % nColors;
				if (label == 0)
				{
					nColorID = nColors;
				}

				if (nValue != 0)
				{
					nHistValue = g_pDepthHist[nValue];

					pDestImage[0] = nHistValue * Colors[nColorID][0]; 
					pDestImage[1] = nHistValue * Colors[nColorID][1];
					pDestImage[2] = nHistValue * Colors[nColorID][2];
				}
				else
				{
					pDestImage[0] = 0;
					pDestImage[1] = 0;
					pDestImage[2] = 0;
				}

				pDepth++;
				pLabels++;
				pDestImage+=3;
			}

			pDestImage += (texWidth - g_nXRes) *3;
		}
	}

	glBindTexture(GL_TEXTURE_2D, depthTexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pDepthTexBuf);

	// Display the OpenGL texture map
	glColor4f(0.75,0.75,0.75,1);

	glEnable(GL_TEXTURE_2D);
	DrawTexture(dmd.XRes(),dmd.YRes(),0,0);	
	glDisable(GL_TEXTURE_2D);

	char strLabel[20] = "";
	XnUserID aUsers[15];
	XnUInt16 nUsers;
	g_UserGenerator.GetUsers(aUsers, nUsers);
	for (int i = 0; i < nUsers; ++i)
	{
		XnPoint3D com;
		g_UserGenerator.GetCoM(aUsers[i], com);
		g_DepthGenerator.ConvertRealWorldToProjective(1, &com, &com);

		if (aUsers[i] == player)
			sprintf(strLabel, "%d (Player)", aUsers[i]);
		else
			sprintf(strLabel, "%d", aUsers[i]);

		glColor4f(1-Colors[i%nColors][0], 1-Colors[i%nColors][1], 1-Colors[i%nColors][2], 1);

		glRasterPos2i(com.X, com.Y);
		glPrintString(GLUT_BITMAP_HELVETICA_18, strLabel);
	}

	// Draw skeleton of user
	if (player != 0)
	{
		glBegin(GL_LINES);
		glColor4f(1-Colors[player%nColors][0], 1-Colors[player%nColors][1], 1-Colors[player%nColors][2], 1);	
		int isSaved = 0;
		int isDrawn = 0;
//		prevNum = imageNum;
		imageNum = get_time();
		isDrawn = DrawLimb(player, XN_SKEL_HEAD, XN_SKEL_NECK);
		isSaved = (isSaved==0)?isDrawn:1;

		isDrawn = DrawLimb(player, XN_SKEL_NECK, XN_SKEL_LEFT_SHOULDER);
		isSaved = (isSaved==0)?isDrawn:1;
		isDrawn = DrawLimb(player, XN_SKEL_LEFT_SHOULDER, XN_SKEL_LEFT_ELBOW);
		isSaved = (isSaved==0)?isDrawn:1;
		isDrawn = DrawLimb(player, XN_SKEL_LEFT_ELBOW, XN_SKEL_LEFT_HAND);
		isSaved = (isSaved==0)?isDrawn:1;

		isDrawn = DrawLimb(player, XN_SKEL_NECK, XN_SKEL_RIGHT_SHOULDER);
		isSaved = (isSaved==0)?isDrawn:1;
		isDrawn = DrawLimb(player, XN_SKEL_RIGHT_SHOULDER, XN_SKEL_RIGHT_ELBOW);
		isSaved = (isSaved==0)?isDrawn:1;
		isDrawn = DrawLimb(player, XN_SKEL_RIGHT_ELBOW, XN_SKEL_RIGHT_HAND);
		isSaved = (isSaved==0)?isDrawn:1;

		isDrawn = DrawLimb(player, XN_SKEL_LEFT_SHOULDER, XN_SKEL_TORSO);
		isSaved = (isSaved==0)?isDrawn:1;
		isDrawn = DrawLimb(player, XN_SKEL_RIGHT_SHOULDER, XN_SKEL_TORSO);
		isSaved = (isSaved==0)?isDrawn:1;

		isDrawn = DrawLimb(player, XN_SKEL_TORSO, XN_SKEL_LEFT_HIP);
		isSaved = (isSaved==0)?isDrawn:1;
		isDrawn = DrawLimb(player, XN_SKEL_LEFT_HIP, XN_SKEL_LEFT_KNEE);
		isSaved = (isSaved==0)?isDrawn:1;
		isDrawn = DrawLimb(player, XN_SKEL_LEFT_KNEE, XN_SKEL_LEFT_FOOT);
		isSaved = (isSaved==0)?isDrawn:1;

		isDrawn = DrawLimb(player, XN_SKEL_TORSO, XN_SKEL_RIGHT_HIP);
		isSaved = (isSaved==0)?isDrawn:1;
		isDrawn = DrawLimb(player, XN_SKEL_RIGHT_HIP, XN_SKEL_RIGHT_KNEE);
		isSaved = (isSaved==0)?isDrawn:1;
		isDrawn = DrawLimb(player, XN_SKEL_RIGHT_KNEE, XN_SKEL_RIGHT_FOOT);
		isSaved = (isSaved==0)?isDrawn:1;

		if(isSaved==1){
//			if(prevNum!=imageNum) { 
			saveDepthImage(dmd);
			saveImage(imd);
//			}
		}
		glEnd();
	}
}
