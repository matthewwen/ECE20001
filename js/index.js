var DRAWER_THRESHOLD = 500;

  var section_selected = '#menu-grading';

  var wind_width = $('#main-content').width();
  var grid_width = $('#grade-input-text').width();
  if (wind_width < grid_width) {
    $('grade-input-text').width(wind_width);
  }

  const exam1 = mdc.textField.MDCTextField.attachTo(document.querySelector('#exam1-textbox'));
  const exam2 = mdc.textField.MDCTextField.attachTo(document.querySelector('#exam2-textbox'));
  const exam3 = mdc.textField.MDCTextField.attachTo(document.querySelector('#exam3-textbox'));
  const homew = mdc.textField.MDCTextField.attachTo(document.querySelector('#homework-textbox'));
  const final = mdc.textField.MDCTextField.attachTo(document.querySelector('#final-textbox'));

  const submit = mdc.ripple.MDCRipple.attachTo(document.querySelector('#submit-button'));
  const dialog = mdc.dialog.MDCDialog.attachTo(document.querySelector('#score-dialog-box'));

  function page_change() {
    mdc.ripple.MDCRipple.attachTo(document.querySelector("#hambergermenu"));
    topAppBar = mdc.topAppBar.MDCTopAppBar.attachTo(document.querySelector("#pkg-top-app-bar"));
    topAppBar.setScrollTarget(document.querySelector('#main-content'));
    const list = mdc.list.MDCList.attachTo(document.querySelector('#side-action-list'));
    list.wrapFocus = true;
    drawer = mdc.drawer.MDCDrawer.attachTo(document.querySelector("#side-action-drawer"));
    window.addEventListener("contextmenu", e => {
      e.preventDefault();
    });
    if (drawer.open == true && window.innerWidth < DRAWER_THRESHOLD) {
      drawer.open = false;
    }
    else if (window.innerWidth < DRAWER_THRESHOLD) {
      drawer.open = false;
    }

    topAppBar.listen('MDCTopAppBar:nav', () => {
      drawer.open = !drawer.open;
    });

    $('#submit-button').click(function () {
      var inte1 = exam1.value;
      var inte2 = exam2.value;
      var inte3 = exam3.value;
      var inthw = homew.value;
      var intfn = final.value;

      var your1 = 0;
      var your2 = 0;

      var total1 = 0;
      var total2 = 0;

      var considered = "<h3>Parameters Considered</h3><ol>";

      if (!isNaN(parseFloat(inte1))) {
        total1 += 20;
        total2 += 25;

        your1 += parseFloat(inte1) * 0.2;
        your2 += parseFloat(inte1) * 0.25;
        considered += "<li>Exam 1</li>"
      }

      if (!isNaN(parseFloat(inte2))) {
        total1 += 20;
        total2 += 25;

        your1 += parseFloat(inte2) * 0.2;
        your2 += parseFloat(inte2) * 0.25;

        considered += "<li>Exam 2</li>"
      }

      if (!isNaN(parseFloat(inte3))) {
        total1 += 20;
        total2 += 25;

        your1 += parseFloat(inte3) * 0.2;
        your2 += parseFloat(inte3) * 0.25;

        considered += "<li>Exam 3</li>"
      }

      var submit3 = total2 == 75;

      if (!isNaN(parseFloat(intfn))) {
        total1 += 25;
        total2 += 35;

        your1 += parseFloat(intfn) * 0.25;
        your2 += parseFloat(intfn) * 0.35;

        considered += "<li>Final</li>"
      }

      if (!isNaN(parseFloat(inthw))) {
        total1 += 15;
        total2 += 10;

        your1 += parseFloat(inthw) * 0.15;
        your2 += parseFloat(inthw) * 0.1;

        console.log(parseFloat(inthw));

        considered += "<li>Homework</li>"
      }
      considered += '</ol>'

      if (submit3 == true) {
        var min = parseFloat(inte1) > parseFloat(inte2) ? parseFloat(inte2) : parseFloat(inte1);
        min = min > parseFloat(inte3) ? parseFloat(inte3) : min;
        total2 = total2 - 25 + 5;

        your2 = your2 - min * 0.25 + min * 0.05;
      }

      var grade = null;
      if (total1 > 0 && total2 > 0) {
        your1 = your1 / total1;
        your2 = your2 / total2;
        grade = your1 > your2 ? your1 : your2;
        grade += 0.02;
        console.log(your1 > your2 ? "option 1" : "option 2");
      }

      if (grade != null) {
        grade = grade * 100;
        considered += "<h3>Final Score: " + grade.toFixed(2) + "% </h3>"
      }
      else {
        considered += "<h3>Final Score: 0% </h3>"
      }

      dialog.open();
      console.log(considered);
      $('#my-dialog-content').html(considered);

    });

  }

  // events when the window changes size
  window.onresize = function () {
    page_change();
  };

  page_change();