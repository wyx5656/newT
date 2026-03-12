    #include <iostream>
    #include <vector>
    #include <list>
    #include <set>
    using namespace std;

    void test()
    {
        vector<int> number = {1,5,5,5,5,5,4};
            // 你的循环结构 + 正确的 erase 写法
        for(auto it = number.begin(); it != number.end(); ) 
        {
            if(5 == *it)
            {
                // ✅ 必须接收返回值，更新 it
                number.erase(it); 
            }
            else
            {
                ++it; // 不删除时才前进
            }
        }

        // 输出结果：1 2 3 4（所有5都被删除）
        for(auto num : number) {
            cout << num << " ";
        }
        return;
    }

    void test1()
    {
        set<int> number = {1,4,8,6,5,56,5,6,8,6};
        size_t cnt = number.count(6);
        cout << endl << "set 的查找" << endl;
        cout << "cnt = " << cnt << endl;

        auto it = number.begin();
        for(; it != number.end(); ++it)
        {
            cout << *it << " ";
        }
        cout << endl;

    }


    int main()
    {
        test1();
    }