from cmsis_stream.cg.scheduler import *

arm2d_tile_type = CStructType("arm_2d_tile_t*",4)

class AmplitudeWidget(GenericNode):
    def __init__(self,name,inLength):
        GenericNode.__init__(self,name)
        self.addInput("i",CType(SINT16),inLength)
        self.addOutput("o",arm2d_tile_type,1)

    @property
    def typeName(self):
        return "AmplitudeWidget"


class Arm2D(GenericSink):
    def __init__(self,nb_tiles=1):
        GenericSink.__init__(self,"Arm2D")



        for i in range(nb_tiles):
            self.addInput(chr(ord('A')+i),arm2d_tile_type,1)


    @property
    def args(self):
        """String of fifo args for object initialization
            with literal argument and variable arguments"""
        allArgs=self.listOfargs
        nb = len(allArgs) 
        others = "{" + "".join(joinit([x.pointer for x in allArgs],",")) + "}"
        # Add specific argrs after FIFOs
        sched = []
        if self.schedArgs:
            for lit in self.schedArgs:
                sched.append(lit.arg)
        if sched:
            return (others + "," + "".join(joinit(sched,",")))
        else:
           return (others)

    def ioTemplate(self):
        """ioTemplate is different for window
        """
        theType=self._inputs[self.inputNames[0]].ctype  
        ios="%s,%d" % (theType,1)
        return(ios)


    @property
    def typeName(self):
        return "Arm2D"

class Signal(GenericSource):
    def __init__(self,name,outLengthA):
        GenericSource.__init__(self,name)
        self.addOutput("o",CType(SINT16),outLengthA)


    @property
    def typeName(self):
        return "Signal"

# USe float instead of float32_t type name

src=Signal("source",256)
amp=AmplitudeWidget("amp",256)
arm2d=Arm2D(nb_tiles=1)

the_graph = Graph()

the_graph.connect(src.o,amp.i)
the_graph.connect(amp.o,arm2d["A"])

# Create a configuration object
conf=Configuration()

#conf.debugLimit=1

conf.CMSISDSP = True
conf.heapAllocation = True
conf.nodeIdentification = True
conf.asynchronous = False
conf.customCName = "lcd_graph_config.h"


with open("pre_simple.dot","w") as f:
    the_graph.graphviz(f)

scheduling = the_graph.computeSchedule(config=conf)

# Print some statistics about the compute schedule
# and the memory usage
print("Schedule length = %d" % scheduling.scheduleLength)
print("Memory usage %d bytes" % scheduling.memory)

scheduling.ccode(".",conf)

# GenericNodes.h is created in the folder "generated"
generateGenericNodes(".")

# cg_status.h is created in the folder "generated"
generateCGStatus(".")

# Generate a graphviz representation of the graph
with open("simple.dot","w") as f:
    scheduling.graphviz(f)
