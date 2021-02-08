let width = 16;
let height = 16;

var isDown = false;
var currentColor;

var finalColors = [];
var finalGrid;

function createGrid(width,height){

	function createSquare(i){
		var div = document.createElement("div");
		div.className = "grid-item";
		//div.innerHTML = `<p>${i}</p>`;
		$(".grid-container").append(div);
	}

  var squareDimension = "auto ";

	$(".grid-container").css({"grid-template-columns":squareDimension.repeat(width),"grid-template-rows":squareDimension.repeat(height)});

	for(i = 0; i < width * height; i++){
		createSquare(i);
	}

	$(document).mousedown(function(){isDown=true;}).mouseup(function(){isDown=false;})

	$(".grid-item").mousedown( function (e) {

				$(this).css({"background-color":currentColor})
	}).mousemove(function (e){

			if(isDown){
				$(this).css({"background-color":currentColor})
			}
	})
}

var colorPicker = document.querySelector("#colorPicker");
colorPicker.addEventListener("change", function(evt) {
  currentColor = evt.target.value;
});


function getGrid(){
	var finalGrid = new Array(height);

	for (var i = 0; i < finalGrid.length; i++){
		finalGrid[i] = new Array();
	}
	var y = 0;

	$(".grid-container").find("div").each(function(){
		var blockColor = $(this).css("background-color")
		blockColor = blockColor.substring(4, blockColor.length-1)
         .replace(/ /g, '')
         .split(',');

		var hex = RGBToHex(parseInt(blockColor[0]),parseInt(blockColor[1]),parseInt(blockColor[2]));

		if(!finalColors.includes(hex)){
			finalColors.push(hex)
		}
		var gridRowLength = finalGrid[y].push(finalColors.indexOf(hex))

		if(gridRowLength == width){
			finalGrid[y].reverse();
			y++;
		}
	})

	data = encodeGrid(finalGrid,finalColors)
	sendGrid(data.colorOutput, data.gridOutput)
}

function RGBToHex(r,g,b) {

	r = r.toString(16)
	g = g.toString(16)
	b = b.toString(16)

	if (r.length == 1){
    r = "0" + r;}
  if (g.length == 1){
    g = "0" + g;}
  if (b.length == 1){
    b = "0" + b;}

  return "#" + r + g + b;
}

function encodeGrid(grid, colors){

	var gridOutput = "";
	var colorOutput = "";

	for (var i = 0; i < colors.length; i++){
		colorOutput = colorOutput + colors[i].substring(1);
	}

	for(var y = grid.length - 1; y > -1; y -= 1){

		for(var x = grid[y].length - 1; x > -1; x -= 1){

			var hex = grid[y][x].toString(16)

			if (hex.length == 1){
				hex = "0" + hex;
			}

			gridOutput = gridOutput + hex
		}
	}

	return {colorOutput, gridOutput}

}

function sendGrid(colors, grid){
	var url = "http://lightboard.local/updateBoard";
	var data = {
		colors: colors,
		grid: grid
	};

	console.log(data);

	$.post(url, data, function(data,status){
		console.log(url, data, status);
	})
}

$(document).ready(function(){
			createGrid(width, height);
});
