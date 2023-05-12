GENERAL INFO
<hr>
BUILD ${build.result}<br>
Build URL: ${rooturl}${build.url}<br>
Project: ${project.name}<br>
Date of build: ${it.timestampString}<br>
Build duration: ${build.durationString}<br>
<br>
<% if(build.result==hudson.model.Result.FAILURE) { %>
<br>
CONSOLE OUTPUT
<hr>
<%
    if(build.workspace.isRemote())
    {
        channel = build.workspace.channel;
        fp = new FilePath(channel, build.workspace.toString() + "/keil.txt")
    } else {
        fp = new FilePath(new File(build.workspace.toString() + "/keil.txt"))
    }
    if(fp != null)
    {
        f = new File(fp.toURI())
        f.eachLine() 
        { line->
%>
            ${line}<br>
<% 	
        }
   } 
}
%>
<br>
<% 
def changeSet = build.changeSet
if(changeSet != null) {
  def hadChanges = false %>
<br>
CHANGE SET
<hr>
<% changeSet.each() { cs ->
     hadChanges = true %><br>
  	 Revision <%= cs.metaClass.hasProperty('commitId') ? cs.commitId : cs.metaClass.hasProperty('revision') ? cs.revision : 
        cs.metaClass.hasProperty('changeNumber') ? cs.changeNumber : "" %> by <%= cs.author %>: (<%= cs.msgAnnotated %>)<br>
<%	 cs.affectedFiles.each() { p -> %>
	 change: <%= p.editType.name %> <%= p.path %><br>
<%   }	 	    
   }
  if(!hadChanges) { %>
  No changes<br>
<% }
} %>
