const arrowsRight = document.querySelectorAll('.arrow-right-container');
const arrowsLeft = document.querySelectorAll('.arrow-left-container');
const movielist = document.querySelectorAll('.movie-list');

var width = Math.round(innerWidth * 7 / 2880);

var indexList = new Array(arrowsRight.length);

for (var i = 0; i < indexList.length; i++) {
    indexList[i] = new Array(3);
    indexList[i][0] = -102;
    indexList[i][1] = 0;
    indexList[i][2] = 0;
}

const itemNumber = 14;

arrowsRight.forEach((arrow, i) => {
    arrow.addEventListener('click', () => {
        width = Math.round(innerWidth * 7 / 2880);
        movielist[i].style.transition = "all .8s";
        if (indexList[i][2] === 2) indexList[i][0] -= 102;
        indexList[i][2] = 0;
        if (itemNumber - (width + indexList[i][1] * width) <= 0) {
            movielist[i].style.transform = "translateX(0)";
            indexList[i][1] = 0; indexList[i][0] = -102;
        }
        else {
            movielist[i].style.transform = `translateX(${indexList[i][0]}%)`;
            indexList[i][0] -= 102; indexList[i][1]++;
        }
        setTimeout(() => {
            (indexList[i][0] === -102) ? arrowsLeft[i].classList.remove('hidden-arrow') : arrowsLeft[i].classList.add('hidden-arrow');}
            , 800);
    });
});

arrowsLeft.forEach((arrow, i) => {
    arrow.addEventListener('click', () => {
        (!indexList[i][2]) ? indexList[i][0] += 204 : indexList[i][0] += 102;
        width = Math.round(innerWidth * 7 / 2880);
        movielist[i].style.transition = "all .8s";
        movielist[i].style.transform = `translateX(${indexList[i][0]}%)`;
        indexList[i][1]--;
        if (!indexList[i][2]) indexList[i][2] = 2;
        setTimeout(() => {
            (indexList[i][0] === 0) ? arrowsLeft[i].classList.remove('hidden-arrow') : arrowsLeft[i].classList.add('hidden-arrow');}
            , 800);
    });
});

const toggle = document.querySelector('.toggle');
const items = document.querySelectorAll(".navbar, .sidebar, .featured-content, .toggle, .boDy");

toggle.addEventListener('click', () => {
    toggle.children[2].classList.toggle('ball-toggle');
    items.forEach(item => item.classList.toggle('active'));
});