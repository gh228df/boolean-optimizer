# boolean-optimizer
A program for optimizing minterms.

The main.cpp is the main algorithm which can be used to generate and optimize a function.

The obool.cpp is an interactive version of main.cpp where you can input your minterms and get the optimized solution. The function consists from English latters, capital letters for non-negated inputs and a lowercase letters for negated inputs. Minterms should be separated by '+' and to calculate the solution you should put '=' after the last minterm.
<h3> Examples </h3>
<h5> Input: AB + A = *enter*</h5>
<h5> Output: A </h5>
<h5> Input: AB + a = *enter*</h5>
<h5> Output: B + a </h5>

# My Algorithm vs Logisim

https://github.com/gh228df/boolean-optimizer/assets/144490574/58af0f20-090f-49a8-9e9d-7b318f1ed733

https://github.com/gh228df/boolean-optimizer/assets/144490574/f3d09487-a686-4eca-916b-835d01c6de11

# Pros and Cons

Optimizing large boolean functions is a complex and resource-intensive task. Generating the shortest possible function is highly demanding due to its complexity. However, this program aims to approximate the optimal solution efficiently, avoiding any O(n^3) operations. Generally, it performs as O(n^2). 

<h3> Pros </h3>
<h5> Efficiently approximates the optimal solution for optimizing boolean functions. </h5>
<h5> Avoids high-complexity operations (O(n^3)), leading to better performance.</h5> 
<h5> Generally performs with a complexity closer to O(n^2).</h5>
<h3> Cons </h3>
<h5> Does not always generate the exact shortest possible function due to the complexity of the task.</h5> 
<h5> Optimizing very large functions can still be challenging and resource-intensive. </h5> 
