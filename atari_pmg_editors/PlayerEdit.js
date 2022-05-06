var table = document.getElementsByTagName('table')[0];
var binaryVals = [0,0,0,0,0,0,0,0];
var bitValues  = [128,64,32,16,8,4,2,1];

table.onclick = function(e) 
{
    var target = (e || window.event).target;
    if (target.tagName in {TD:1, TH:1})
    {
		var rIndex = target.parentElement.rowIndex;
		var cIndex = target.cellIndex+1;

		if( cIndex > 9 || cIndex < 2 || rIndex < 1 )
			return;

		var cellValue = parseInt(table.rows[rIndex].cells[9].innerHTML);

    	var style = target.getAttribute('style');
    	if( target.style.backgroundColor != "red")
    	{
    		cellValue += bitValues[cIndex-2];
    		table.rows[rIndex].cells[9].innerHTML = cellValue.toString();
    		table.rows[rIndex].cells[10].innerHTML = "0x"+cellValue.toString(16).toUpperCase();
    		table.rows[rIndex].cells[11].innerHTML = "0b"+cellValue.toString(2).toUpperCase();
    		target.setAttribute('style', 'background-color: red');
    	}
    	else
    	{
    		cellValue -= bitValues[cIndex-2];
    		table.rows[rIndex].cells[9].innerHTML = cellValue.toString();
    		table.rows[rIndex].cells[10].innerHTML = "0x"+cellValue.toString(16).toUpperCase();
    		table.rows[rIndex].cells[11].innerHTML = "0b"+cellValue.toString(2).toUpperCase();
    		target.setAttribute('style', 'background-color: white');
    	}
    }
};