#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef struct
{
    int id;
    char name[25];
    int chinese;
    int math;
    int english;
} Student;

void addStuInfo(Student* student, int stuNum)
{   
    int i = 0;
    for(i = 0; i < stuNum; i++)
    {
        printf("请输入学生id : \n");
        scanf("%d", &student->id);
        printf("请输入学生姓名 : \n");
        scanf("%24s", student->name);
        printf("请输入学生语文成绩 : \n");
        scanf("%d", &student->chinese);
        printf("请输入学生数学成绩 : \n");
        scanf("%d", &student->math);
        printf("请输入学生英语成绩 : \n");
        scanf("%d", &student->english);
        student++;
    }
}

void printStuInfo(Student* student, int stuNum)
{   
    int i = 0;
    for(i = 0; i < stuNum; i++)
    {
        printf("学生id : %d, 学生姓名: %s, chinese: %d, math: %d, english %d\n", 
                student->id, student->name, student-> chinese, student->math, student->english);
        student++;
    }
}

int stuCompare(const void* a, const void* b)
{
    Student* stu1 = (Student*)a;
    Student* stu2 = (Student*)b;
    int total1 = stu1->chinese + stu1->math + stu1->english;
    int total2 = stu2->chinese + stu2->math + stu2->english;
    if(total1 != total2)
    {
        return total2 - total1;
    }

    if(stu1->chinese != stu2->chinese)
    {
        return stu2->chinese - stu1->chinese;
    }
    
    if(stu1->math != stu2->math)
    {
        return stu2->math - stu1->math;
    }

    if(stu1->english != stu2->english)
    {
        return stu2->english - stu1->english;
    }

    return strcmp(stu1->name, stu2->name);
}

int stuCompareMe(Student* student, int stuNum, int (*compare_func)(const void* a, const void *b));

void sortStuInfo(Student* student, int stuNum)
{ 
    qsort(student, stuNum, sizeof(Student), stuCompare);
}

int main()
{
    int stuNum = 0;
    Student student[100];
    printf("输入学生数量：\n");
    scanf("%d", &stuNum);
    //初始化学生成绩
    addStuInfo(student, stuNum);
    //打印当前学生信息
    printStuInfo(student, stuNum);

    printf("排序后的学生信息：\n");
    sortStuInfo(student, stuNum);
    printStuInfo(student, stuNum);

}