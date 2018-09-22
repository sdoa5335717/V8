/************************************************************************/
/*��������
/*�����������HLSL����
/************************************************************************/
#pragma once
struct Vertex;
class XShader;
class XMesh
{
public:
	XMesh();
	~XMesh();

	void CreateMesh(DWORD NumFaces,
		DWORD Options, DWORD FVF, const std::vector<Vertex>& vArray,
		const std::vector<WORD>& iArray, LPDIRECT3DDEVICE9 pDevice);

	void SetSubset(int nStart, int nEnd, DWORD id);

	void ReadMeshFromFile();

	void DrawSubset(int i = -1);

private:
	// ��ɫ��
	//XShader _shader;
	// 
	ID3DXMesh* _pMesh = NULL;
};

