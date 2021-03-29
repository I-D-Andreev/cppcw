# Compiler
g++ (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0  

# Notes
The notes below will explain places where I feel the code could have been better, but ultimately ended as is due to various constraints.  

## Exceptions
### - catching std::exception
There are some places in the code where only a limited subset of exceptions can be thrown (e.g. either out_of_range or runtime_error only). Ideally, I would like to catch the different types, but as the outcome is always the same, I decided to catch the base type exception to avoid repetitions.

### - BethYw::run
The BethYw::run function uses a try-catch around the whole code, which I don't feel is particularly good.  

I would have liked to handle the different logical blocks (like args reading, args parsing, data loading, etc.) in different sections, but in that case the variables would have been scoped to the try {} blocks and the alternative of having all of them declared outside empty and assigned later looked quite messy (also some variables as cxxopts::ParseArg could not be delcared empty, copy constructed or moved..).  

In the end, the code was a lot easier to understand when I used a single try-catch with everything inside, including functions that do not throw.  

Finally, I at least wanted to catch the different exceptions, such as cxxopts::OptionSpecException, cxxopts::OptionParseException, std::invalid_argument, std::runtime_error, etc.. and output a slightly different std::cerr for each, **but** it turned out that one of the output tests was failing due to the additional text, so I decided that the safest option would be to just catch only the base type exception and only print the cause.

## Areas.cpp - ::lowerCaseFilter
Certain functions take const StringFilterSet* const pointer and you will sometimes see this slow function, which copies the set from the pointer and returns the whole of it in lowercase. Initially, this was used for both the areas and measurements, but after implementing task 8 (search by partial name), only measurements use it now.  

The problem is that although we use a set of strings, which should guarantee string uniqueness, the set is case sensitive. So, if I had a measure rails, I could add "rAils", "RAILS", "rails", etc.., which would result in duplication. On the other end of the rope, if we tried to search "Rails" in the abovementioned set, the answer would be that the set does not contain "Rails". However, the problem statement/ block comments expect the program args to be case-insensitive.  

The ideal solution would be for us to directly save the arguments as lowercase in the parseArg functions, but there is a unit test that specifically expects a value that contains uppercase to be returned without being changed (this issue was also mentioned in the Canvas forum discussion).  

As the number of arguments would never be really big, I felt that although seemingly slow, this was better solution than duplicating the data in lowercase in another variable.

## Pretty print JSON
When printing the JSON output in readable format, the CS-Autograder JSON tests fail, so I have left the line of code responsible for that half-commented out. The particular line is in areas.cpp, line 929.  

I have also left an executable build for pretty JSON printing: ./bin/bethyw_json_pretty


## map / unoredered_map comments legend
You will see a few comments like:

// {code : Area}  
std::map <std::string, Area> m;

The braces on top specify what the elements in the map represent, the first one being what the key is and the second one what the value is.  
I.e.  
// {key = area code : value = Area itself}

## Code formatting
I have formatted my whole code with CLion. I tried to tinker with the indentation to make it as close as possible to the original, but there are some places (especially in some function definitions) where I find the spacing slightly weird. Nonetheless, I have not changed them so that all the code would look similar (and because I guess there is some format that uses such spacing if CLion is using it).
