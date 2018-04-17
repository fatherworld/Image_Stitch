#ifndef _Flow_h_
#define _Flow_h_

typedef struct _pairImage_
{
    int imageIndex;
    float* H;
}pairImage;

typedef struct _Node_
{
    pairImage* m;
    struct _Node_* next;
}node;

typedef struct _Node_Head_
{
    int count;
    node* next;
}NodeHead;

#ifdef __cplusplus
extern "C"
{
#endif

    node* instancePariImage();
    void initListHead(NodeHead* listHead);
    void headInsert(NodeHead* listHead, node* newNode);
    node* getNode(NodeHead* listHead, int nodeIndex);
    void destroyList(NodeHead* listHead);

    int ISComplementH(int number, HMatrixInfo* mHMatrixInfo, HMatrixInfo* mOutHMatrixInfo);
#ifdef __cplusplus
};
#endif

#endif