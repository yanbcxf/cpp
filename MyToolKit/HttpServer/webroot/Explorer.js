$(document).ready(function(){

  
  // $("#demo").height(400);
 
	tree1 = new tree_component();
	tree1.init($("#demo"), { 
		data : {
			type  : "json",
			async : true,
			url   : "Explorer.xml?Option=GetJsonTree"
		},
		callback : {
			ondblclk : function(NODE, TREE_OBJ) { 
				TREE_OBJ.toggle_branch.call(TREE_OBJ, NODE); 
				TREE_OBJ.select_branch.call(TREE_OBJ, NODE); 
				if(NODE.className!="open")
					OnSelectPath();
			}
		} 
	});
  
	$("#BtnOk").click(OnSelectPath);
	
});

function OnSelectPath()
{
	if(tree1.selected!=undefined)
	{
		self.parent.SelectPath($(document).getUrlParam("obj"), tree1.selected[0].id);
	}
	self.parent.tb_remove();	
}