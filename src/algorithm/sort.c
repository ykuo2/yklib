//
//  sort.c
//  yklib
//
//  Created by Yu Kuo on 2026/8/14.
//

#include <yklib/algorithm/sort.h>

#include <stdlib.h>
#include <string.h>

// 2026/8 quick sort
// generic, Hole-filling partition, pivot strategy: always select left most
// pivot number: malloc each time the function is called
// TODO: Consider reusing a single buffer to avoid repeated allocations.
// TODO: use other partition, pivot stategy
void yk_quick_sort(void *base, size_t n_member, size_t size, yk_compare_fn compare){
    // generic 版本和 array 版本的差異：
    // 使用 void *base 指向要排序的起點，利用 n_member 以及 size 來換算邊界
    // 每一次排序結束後再重新分配 base, n_number
    
    if (n_member<=1){  // n_member 0 or 1-> already sorted
        return;
    }
    // low, high 為此次 recursive call 的固定區間邊界，不變動
    // left, right 為目前掃描位置，在演算法中移動
    unsigned char *arr = base;  // void 轉成 unsigned char，才能合法進行指標運算
    size_t left = 0;
    size_t right = n_member-1;
    size_t middle;  // 最後 pivot 收斂處，where scan_left==scan_right
    
    // 暫存，“抽出” pivot member
    void *pivot = malloc(size);
    if (pivot==NULL){
        return;
    }
    memcpy(pivot, arr, size);
    
    // 因此首次 hole on left; 首次 scan on right
    while (left<right){
        // NB: when left==right，arr[left]=arr[right] / arr[right]=arr[left] 不會有任何 effect
        // hole on left, scan on right
        while (left<right && compare(arr+right*size, pivot)>0){
            // left<right 且 right>pivot，移動 index 找下一個 right
            right--;
        }
        // right<pivot，將 right 移動到 left hole
        if (left<right){  // 避開 left==right，因為已經收斂，且 memcpy 為 UB
            memcpy(arr+left*size, arr+right*size, size);
        }
        // hole on right, scan on left
        while (left<right && compare(arr+left*size, pivot)<=0){
            // left<right 且 left<=pivot，移動 index 找下一個 left
            left++;
        }
        // left>pivot，將 left 移動到 right hole
        if (left<right){  // 避開 left==right，因為已經收斂，且 memcpy 為 UB
            memcpy(arr+right*size, arr+left*size, size);
        }
    }
    
    middle = left;  // right 也可以，此時 right==left
    memcpy(arr+middle*size, pivot, size);
    free(pivot);

    // 重新分組，分配 base, n_number
    yk_quick_sort(arr, middle, size, compare);
    yk_quick_sort(arr+(middle+1)*size, n_member-middle-1, size, compare);
}
