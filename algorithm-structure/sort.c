#include <stdio.h>

void swap(int *a,int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

void BubbleSort(int num[],int n){
    for (int i = n;i >= 0; i--) {
        for ( int j = 0;j < i; j++) {
            if (num[j] > num[j + 1]) {
                swap(&num[j + 1],&num[j]);
            }
        }
    }
}


void EasySort(int num[],int n){
    int min;
    for (int i = 0; i < n; i++)
    {
        min = i;
        for (int j = i; j < n; j++)
        {
            if (num[min] > num[j])
            {
                min = j;
            }
        }
        if (min != i)
        {
            swap(&num[i],&num[min]);
        }
    }
}


void InsertSort(int num[],int n) {
    for (int i = 1;i < n; i++) {
        int temp = num[i];
        int j = i;
        while (j - 1 >= 0 && num[j - 1] > temp)
        {
            num[j] = num[j - 1];
            j--;
        }
        num[j] = temp;
    }
}


void ShellSort(int num[],int n){
    int gra = n;
    do
    {
        gra = gra / 2;
        for (int j = 0; j < gra; j++)
        {
            for (int i = j; i < n - 1 - j; i += gra)
            {
                if (num[i] > num[i + gra])
                {
                    swap(&num[i], &num[i + gra]);
                }
            }
        }
    } while (gra > 1);
}


void FastSort(int num[], int left, int right) {

    if(left > right) return;
    
    int begin = left;
    int end = right;
    while(num[end] > num[left] && end > begin) {
        end--;
    }
    if(end > begin) swap(&num[left], &num[end]);
    while(num[begin] < num[end] && begin < end) {
        begin++;
    }
    if(begin < end) swap(&num[begin], &num[end]);

    FastSort(num, left, begin-1);
    FastSort(num, begin, right);


   
}


int main(int argc, char const *argv[]) {
    int n = 5;
    //scanf("%d",&n);
    //2 8 5 1 3 
    int num[n];
    for (int i = 0; i < n; i++)
    {
        scanf("%d",&num[i]);
    }
    
    if (n != 1) FastSort(num, 0, n);

    for (int i = 0; i < n; i++)
    {
        printf("%d",num[i]);
        if (i != n-1) printf(" ");
    }
    printf("\n");

    return 0;

}