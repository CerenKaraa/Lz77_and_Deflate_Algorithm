#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
int say=0;
struct token{
    uint8_t offset;
    char c;
};
struct harfler{
    char c;
    int tekrar;
};
struct Node {
    char data;
    int tekrar;
    struct Node *left,*right;
};
struct minheap{
    int boyut;
    int kapasite;
    struct Node** nodes;
};
struct Node* newNode(char c,int tekrar)
{
    struct Node* temp=(struct Node*)malloc(sizeof(struct Node));
    temp->left=NULL;
    temp->right=NULL;
    temp->data=c;
    temp->tekrar=tekrar;

    return temp;
}
void minHeap(struct minheap* mh,int index)
{
    int min=index;
    int left=index+1;
    int right=index+2;

    if (right<mh->boyut && mh->nodes[right]->tekrar<mh->nodes[min]->tekrar)
        min=right;
    if (left<mh->boyut && mh->nodes[left]->tekrar<mh->nodes[min]->tekrar)
        min=left;
    if (min!=index) {
     struct Node*t=mh->nodes[min];
     mh->nodes[min]=mh->nodes[index];
     mh->nodes[index]=t;
     minHeap(mh,min);
    }
}
struct Node* minbul(struct minheap* mh)
{
    struct Node*temp=mh->nodes[0];
    mh->nodes[0]=mh->nodes[mh->boyut-1];
    mh->boyut--;
    minHeap(mh,0);

    return temp;
}

void ekle(struct minheap* mh,struct Node* node)
{
    int i;
    mh->boyut++;
    for(i=mh->boyut-1;node->tekrar<mh->nodes[(i - 1)/2]->tekrar;((i - 1)/2)){
            mh->nodes[i]=mh->nodes[(i-1)/2];
    }
    mh->nodes[i]=node;
}
struct minheap* Heapolustur(char harf[],int tekrar[],int boyut)
{
    struct minheap* mh=(struct minheap*)malloc(sizeof(struct minheap));
    mh->boyut=0;
    mh->kapasite=boyut;
    mh->nodes=(struct Node**)malloc(mh->kapasite*sizeof(struct Node*));
    int i;
    for (i=0;i<boyut;i++){
        mh->nodes[i]=newNode(harf[i],tekrar[i]);
    }
    mh->boyut=boyut;
    for(i=(mh->boyut-2)/2;i>=0;i--)
        minHeap(mh,i);

    return mh;
}

struct Node* HuffmanAgac(char harf[],int tekrar[],int boyut)
{
    struct Node *left,*right,*top;
    struct minheap*mh=Heapolustur(harf,tekrar,boyut);

    while (mh->boyut!=1) {

        left=minbul(mh);
        right=minbul(mh);

        top = newNode('a',left->tekrar+right->tekrar);

        top->left=left;
        top->right=right;
        ekle(mh,top);
    }
    return minbul(mh);
}

void sonucyaz(struct Node* root,int a[],int top)
{
    FILE *fp=fopen("deflate.txt","a");
    int i;
    if (root->left) {
        a[top]=0;
        sonucyaz(root->left,a,top+1);
    }
    if (root->right) {
        a[top]=1;
        sonucyaz(root->right,a,top+1);
    }
    if (!(root->left) && !(root->right)) {
        printf("%c: ",root->data);
        fputc(root->data,fp);
        for (i=0;i<top;i++){
        printf("%d", a[i]);
        fputc(a[i],fp);
        }
    printf("\n");
    fputc("\n",fp);
    fclose(fp);
    }
}
void HuffmanHesapla(char harf[],int tekrar[],int boyut)
{
    int a[100],top=0;
    struct Node* root= HuffmanAgac(harf,tekrar,boyut);
    sonucyaz(root,a,top);
}
int eslesmebul(char *c1,char *c2,int max)
{
    int boy=0;

    while(*c1++==*c2++ && boy<max)
        boy++;

    return boy;
}
struct token *kodla(char *metin,int limit,int *boyut)
{
    int cap=1<<3;
    struct token t;
    char *ileri,*ara;
    struct token *kodlanmis=malloc(cap*sizeof(struct token));
    for (ileri=metin;ileri<metin+limit;ileri++)
    {
        ara=ileri-(1<<(5))-1;
        if (ara<metin)
            ara=metin;
        int max=0;
        char *esit=ileri;
        for (;ara<ileri;ara++)
        {
            int len=eslesmebul(ara,ileri,(1<<(3))-1);
            if (len>max)
            {
                max=len;
                esit=ara;
            }
        }
        if (ileri+max>=metin+limit)
        {
            max=metin+limit-ileri- 1;
        }
        t.offset=(ileri-esit)<<3|max;
        ileri+=max;
        t.c=*ileri;
        if (say+1> cap)
        {
            cap=cap<<1;
            kodlanmis=realloc(kodlanmis,cap*sizeof(struct token));
        }
        kodlanmis[say++]=t;
    }
    if(boyut)
        *boyut=say;

    return kodlanmis;
}
char *dosyaoku(FILE *dosya,int *boyut)
{
    char *bilgi;
    fseek(dosya,0,SEEK_END);
    *boyut=ftell(dosya);
    bilgi=malloc(*boyut);
    fseek(dosya,0,SEEK_SET);
    fread(bilgi,1,*boyut,dosya);
    return bilgi;
}

int main()
{
    FILE *dosya;
    int metinboyut=8,tokensay=0,i,harfsay=0,j,kontrol=0;
    char harf[100];
    int tekrar[100],tut[100];
    char *kaynak;
    struct token *kodlanmis;
    if (dosya=fopen("metin.txt","r"))
    {
        kaynak=dosyaoku(dosya,&metinboyut);
        fclose(dosya);
    }
    kodlanmis=kodla(kaynak,metinboyut,&tokensay);
    struct harfler *h = (struct harfler*)malloc(sizeof(struct harfler));
    for(i=0;i<tokensay;i++){
        if(harfsay==0){
            h[harfsay].c=kodlanmis[i].c;
            h[harfsay].tekrar=1;
            harfsay++;
        }
        else{
            for(j=0;j<harfsay;j++){
                if(h[j].c==kodlanmis[i].c){
                    h[j].tekrar++;
                    kontrol=1;
                }
            }
            if(kontrol==0){
                h=realloc(h,sizeof(struct harfler)*(harfsay+1));
                h[harfsay].c=kodlanmis[i].c;
                h[harfsay].tekrar=1;
                harfsay++;
            }
        }
        kontrol=0;
    }
    if (dosya=fopen("lz77.txt","wb"))
    {
        fwrite(kodlanmis,sizeof(struct token),tokensay,dosya);
        fclose(dosya);
    }
    FILE *fp= fopen("lz77.txt","ab");
    printf("\nLZ77= Orjinal Boyut: %d, Encode Boyutu: %d\n", metinboyut,tokensay*sizeof(struct token));
	fclose(fp);
    for(i=0;i<harfsay;i++){
      tekrar[i]=h[i].tekrar;
      harf[i]=h[i].c;
    }
    HuffmanHesapla(harf,tekrar,harfsay);
    printf("LZ77.txt ve Deflate.txt olusturulmustur.Lutfen kontrol ediniz.");
    return 0;
}
