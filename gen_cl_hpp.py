#!/usr/bin/env python
# Generation for the 1.x version of cl.hpp where expansion of variadic interfaces is required.

import re
import sys
from optparse import OptionParser

parser = OptionParser()
parser.add_option('-i', '--input', dest = 'input_filename', metavar = 'FILE', default = 'input_cl.hpp', help = 'Template file')
parser.add_option('-o', '--output', dest = 'output_filename', metavar = 'FILE', default = 'cl.hpp', help = 'Output file')
(options, args) = parser.parse_args()
if len(args) > 0:
    parser.error("No arguments expected")

inputCLHPP = open(options.input_filename, 'r')
contents = inputCLHPP.read()
inputCLHPP.close()

outputCLHPP = open(options.output_filename, 'w')

numArgs = 32


def genTypeNameList(numEntries):
    typeNameString = ""
    for i in range(0, numEntries):
        typeNameString = typeNameString + "\ttypename T" + str(i)
        if i < (numEntries-1):
            typeNameString = typeNameString + ",\n"
    return typeNameString;

def genTemplateParameterList(numArguments, totalArguments, indent):
    fullArgList = ""
    for i in range(0, numArguments):
        fullArgList = fullArgList + indent + "T" + str(i)
        if i < (totalArguments-1):
            fullArgList = fullArgList + ",\n"
    return fullArgList

def genTemplateParameterListCommaFirst(numArguments, totalArguments):
    fullArgList = ""
    for i in range(0, numArguments):
        fullArgList = fullArgList + ",\n\t\tT" + str(i)
    return fullArgList

def genTemplateArgumentList(numArguments, totalArguments):
    fullArgList = ""
    for i in range(0, numArguments):
        fullArgList = fullArgList + ",\n\t\tT" + str(i) + " arg" + str(i)
    return fullArgList

def genFunctionArgumentList(numArguments, totalArguments):
    fullArgList = ""
    for i in range(0, numArguments):
        fullArgList = fullArgList + ",\n\t\t\targ" + str(i)
    return fullArgList

def genFullArgList(numArguments, totalArguments, indent):
    fullArgList = genTemplateParameterList(numArguments, totalArguments, indent)

    for i in range(0, totalArguments - numArguments):
        fullArgList = fullArgList + indent + "NullType"
        if i < (totalArguments - numArguments - 1):
            fullArgList = fullArgList + ",\n"
    return fullArgList

def genSpecialization(numArguments, totalArguments):
    specialization = ""
    specialization = "template<\n" + genTypeNameList(numArguments) + ">\nstruct functionImplementation_\n"
    
    if( numArguments < totalArguments ):
        specialization = specialization + "<" + genFullArgList(numArguments, totalArguments, "\t") + ">\n"
    specialization = specialization + "{\n\ttypedef detail::KernelFunctorGlobal<\n"
    specialization = specialization + genFullArgList(numArguments, totalArguments, "\t\t") + "> FunctorType;\n"

    specialization = specialization + """
    FunctorType functor_;

    functionImplementation_(const FunctorType &functor) :
        functor_(functor)
    {
    """

    # Insert a compile-time check to assert on Visual Studio 2012 if we have more than 10
    # arguments in the variadic expansion
    if totalArguments > 5:
        specialization = specialization + """
        #if (defined(_WIN32) && defined(_VARIADIC_MAX) && (_VARIADIC_MAX < """ + str(numArguments) + """))
        // Fail variadic expansion for dev11
        static_assert(0, "Visual Studio has a hard limit of argument count for a std::function expansion. Please define _VARIADIC_MAX to be 10. If you need more arguments than that VC12 and below cannot support it.");
        #endif
        """
    specialization = specialization + """    
    }

	//! \\brief Return type of the functor
	typedef Event result_type;

	//! \\brief Function signature of kernel functor with no event dependency.
	typedef Event type_(
		const EnqueueArgs&""" 
    specialization = specialization + genTemplateParameterListCommaFirst(numArguments, numArguments) + ");\n\n"

    specialization = specialization + "\tEvent operator()(\n\t\tconst EnqueueArgs& enqueueArgs" + genTemplateArgumentList(numArguments, numArguments) + ")\n"
    specialization = specialization + "\t{\n\t\treturn functor_(\n\t\t\tenqueueArgs" + genFunctionArgumentList(numArguments, numArguments) + ");\n\t}\n"

    specialization = specialization + """

};

"""

    return specialization



functionImplementation = ""
for numParameters in range(numArgs, 0, -1):
    functionImplementation = functionImplementation + genSpecialization(numParameters, numArgs)

outputContents = contents.replace("%FUNCTION_IMPLEMENTATION_REPLACEMENT_POINT%", functionImplementation)

outputContents = re.sub("//@@@@EXCESSIVE-ARG-START.*?[\n\r](.*?[\n\r])+?//@@@@EXCESSIVE-ARG-END.*?[\n\r]", "", outputContents, 0, re.MULTILINE)

outputCLHPP.write(outputContents)
outputCLHPP.close()
