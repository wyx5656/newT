#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}
void printArray(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}
//选择排序，选择每一次遍历，都选择最大/最小的值放在已排序数组的末尾，时间复杂度O(n2) 空间复杂度O(1)
void SeletionSort(int a[], int n)
{ 
    for(int i = 0; i < n - 1; i++)
    {
        int min = i;
        for(int j = i + 1; j < n; j++)
        {
            if(a[j] < a[min])
            {
                min = j;
            }
        }

        if(a[min] < a[i])
        {
            swap(&a[min], &a[i]);
        }
    }
}
// 冒泡排序，每轮比较相邻的元素，将较大的冒到最后去。时间复杂度O（n2）最好O（1），空间复杂度O1
void bubbleSort(int a[], int n)
{   
    for (int i = 0; i < n - 1; i++)
    {
        bool swapped = false;
        for (int j = 0; j < n - 1 -i; j++)
        {
            if (a[j] > a[j+1])
            {
                swap(&a[j], &a[j+1]);
                swapped = true;
            }
        }
        
        if(false == swapped)
        {
            printf("第 %d 轮后已经有序，提前结束\n", i + 1);
            return;
        }
    }
}
//插入排序，前m为有序数组，后n为无序数组，每次从无序数组的第一个，插入到有序数组中，使 m + 1 有序，
// n -1 仍为无序。时间复杂度最好为O(n) 最差还是O(N2),空间还是O(1)
void insertSort(int a[], int n)
{
    for(int i = 1; i < n; i++)
    {
        int key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key)
        {
            a[j + 1] = a[j];
            j--;
        }

        a[j + 1] = key;
    }
}

// 分组进行插入排序，gap = n / 2 组， 
//第一组 0 gap
//第二组 1 gap + 1
//第gapzu1 gap  gap + gap

//第二轮使用 gap / 2 作为组， 直到 gap = 1
//使数组基本有序，将较小元素往前挪，降低插入排序的移动次数
//时间复杂度 和gap 有关，一般小于On2 最好可达到O nlogn 空间复杂度O1
void shellSort(int a[], int n)
{
    for (int gap = n / 2; gap > 0; gap /=2)
    {   
        printf("当前 gap = %d\n", gap);
        for (int i = gap; i < n; i++)
        {
            int key = a[i];
            int j = i - gap;
            while(j >= 0 && a[j] > key)
            {
                a[j + gap] = a[j];
                j = j - gap;
            }

            a[j + gap] = key;
        }
        // 打印每一轮后的数组状态
        printf("本轮结束后: ");
        for (int k = 0; k < n; k++) printf("%d ", a[k]);
        printf("\n\n");
    }
}
//归并排序  把大数组不断拆成小数组，直到每个小数组只有一个元素，天然有序，然后两两合并成有序数组，最终
//得到完整的有序序列
//过程可以分为两个阶段：
//拆分（Divide）：递归地把数组分成两半
//合并（Conquer & Merge）：把两个有序的小数组合并成一个更大的有序数组
//时间复杂度O nlogn 空间复杂度 On
void merge(int arr[], int left, int mid, int right)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;
    int *L = (int*)malloc(n1 * sizeof(int));
    int *R = (int*)malloc(n2 * sizeof(int));
    for(int i = 0; i < n1; i++)
    {
        L[i] = arr[left + i];
    }
    for(int j = 0; j < n2; j++)
    {
        R[j] = arr[mid + 1 +j];
    }

    //合并回原数组
    int i = 0, j = 0, k = left;
    while(i < n1 && j < n2)
    {
        if(L[i] <= R[j])
        {
            arr[k++] = L[i++];
        }
        else
        {
            arr[k++] = R[j++];
        }
    }

    while(i < n1) arr[k++] = L[i++];
    while(j < n2) arr[k++] = R[j++];
    free(L);
    free(R);
}

void mergeSort(int arr[], int left, int right)
{
    if(left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}
//快速排序：时间复杂度 O nlogn 最坏 On2 总是选到极值 空间复杂度O logn 递归栈
//快速排序通过选择 pivot、分区、递归处理左右两部分实现排序
//，平均 O(n log n)，原地、不稳定，是实际应用中最快的通用排序算法之一。
int partition(int arr[], int low, int high)
{
    int pivot = arr[high];
    int i = low -1;
    for (int j = low; j < high; j++)
    {
        if(arr[j] <= pivot)
        {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }

    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

void quickSort(int arr[], int low, int high)
{
    if(low < high)
    {
        // 分区，得到privot的正确位置
        int pi = partition(arr, low, high);
        //递归处理左部分
        quickSort(arr, low, pi - 1);
        //递归处理右部分
        quickSort(arr, pi + 1, high);
    }

}

// 包装函数（方便调用）
void sort(int arr[], int n) {
    quickSort(arr, 0, n - 1);
}
/*
堆排序（Heap Sort） 是基于二叉堆（Binary Heap）的一种原地、不稳定的排序算法，时间复杂度稳定在 O(n log n)，是少数几种“最坏情况也 O(n log n)”的排序算法之一。
1. 核心思想（一句话记住）
先把整个数组建成一个最大堆（或最小堆），然后反复把堆顶（最大/最小元素）取出放到数组末尾，调整剩余部分成新堆，直到所有元素有序。
2. 堆排序的两个关键步骤

建堆（Build Heap）
把无序数组调整成一个最大堆（或最小堆）。
最坏 O(n)，平均 O(n)（不是 O(n log n)！这是堆排序的一个优化点）。
排序（Heap Sort）
反复执行：
把堆顶（最大元素）放到数组末尾
把堆的大小减 1
对剩余部分重新调整成最大堆（Heapify）

堆排序通过先建最大堆，再反复把堆顶放到数组末尾并调整剩余部分实现排序，时间稳定 O(n log n)，空间 O(1)，原地、不稳定，是少数几种最坏情况也 O(n log n) 的排序算法。
最大堆的性质（必须记住）
对于任意节点 i：

arr[i] ≥ arr[2i+1]（左子节点）
arr[i] ≥ arr[2i+2]（右子节点）（如果存在）

根节点是最大值。
*/
//维护最大堆性质，n 数组当前的有效长度，当前要调整的节点索引

//假设以某个节点 i 为根的子树，除了根节点本身外，其他部分已经满足最大堆性质，现在只需要对根节点进行调整，让以 i 为根的整棵子树恢复最大堆性质。
void heapify(int arr[], int n, int i)
{
    while(i < n)
    {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        if (left < n && arr[left] > arr[largest])
        {
            largest = left;
        }

        if (right < n && arr[right] > arr[largest])
        {
            largest = right;
        }

        if (largest == i) break;
        swap(&arr[i], &arr[largest]);
        i = largest;
    }
}


static void buildHead(int arr[], int n)
{
    //构建最大堆，从最后一个非叶子节点开始，逐个调用heapify
    for (int i = n / 2 - 1; i >= 0; i--)
    {
        heapify(arr, n, i);
    }
}
void heapSort(int arr[], int n)
{
    //构建大顶堆
    buildHead(arr, n);

    int len = n;
    //每次把堆顶放到末尾，并调整剩余部分
    while(len > 1)
    {
        swap(&arr[0], &arr[len - 1]);
        len--;
        heapify(arr,len, 0);
    }
}

int binarySearch(int arr[], int n, int key)
{
    int left = 0; int right = n - 1;
    while(left <= right)
    {
        int mid = left + (right - left) / 2;
        if(key < arr[mid])
        {
            right = mid - 1;
        }
        else if(key > arr[mid])
        {
            left = mid + 1;
        }
        else{
            return mid;
        }
    }

    return -1;
}
int main()
{
    int arr[] = {64, 25, 12, 22, 11, 23, 25, 26};
    int n = sizeof(arr) / sizeof(arr[0]);
    printf("原始数组：\n");
    printArray(arr, n);
    heapSort(arr, n);
    printf("排序后的数组 : \n");
    printArray(arr, n);

    int a = binarySearch(arr,n,50);
    printf("22的位置是 %d\n", a);
}

