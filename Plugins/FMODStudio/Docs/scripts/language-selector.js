var languageTabs = null;
var languageSpecificElements = null;
var availableLanguages = null;

// Helper function to iterate over an HTMLCollection or NodeList (https://stackoverflow.com/questions/3871547/js-iterating-over-result-of-getelementsbyclassname-using-array-foreach)
function forEachElement(collection, func) {
  Array.prototype.forEach.call(collection, func);
}

function matchLanguage(el, lang) {
  if (lang == "language-all" || el.classList.contains("language-all")) {
    return true;
  } else if ((lang === "language-c" || lang === "language-cpp") && el.classList.contains("language-c-cpp")) {
    return true;
  } else {
    return el.classList.contains(lang);
  }
}

function setLanguage(lang) {
  if (languageSpecificElements == null) {
    return;
  }

  window.localStorage.setItem("FMOD.Documents.selected-language", lang);

  if (availableLanguages.length > 0 && !availableLanguages.includes(lang)) {
    lang = availableLanguages[0];
  }

  forEachElement(languageTabs, function(el) {
    var ellang = el.attributes['data-language'].value;
    
    if (ellang === lang) {
      el.classList.add("selected");
    } else {
      el.classList.remove("selected");
    }
  });

  forEachElement(languageSpecificElements, function(el) {
    if (matchLanguage(el, lang)) {
      el.style.display = 'block';
    } else {
      el.style.display = 'none';
    }
  });

  window.localStorage.setItem("FMOD.Documents.selected-language", lang);
}

function init() {
  var docsBody = document.querySelector("div.manual-content.api");

  if (docsBody) {
      // API docs

      // Setup language tabs
      languageTabs = docsBody.getElementsByClassName("language-tab");

      forEachElement(languageTabs, function(el) {
        el.onclick = function() { setLanguage(this.attributes['data-language'].value); }
      });

      // Cache language specific elements on the page
      languageSpecificElements = docsBody.querySelectorAll(".language-c, .language-cpp, .language-c-cpp, .language-csharp, .language-javascript");

      // Determine languages used on the page
      availableLanguages = [];
      ["language-c", "language-cpp", "language-c-cpp", "language-csharp", "language-javascript"].forEach(function(lang) {
        if (docsBody.querySelector("." + lang) != null) {
          availableLanguages.push(lang);
        }
      });
      
      if (availableLanguages.indexOf("language-c-cpp") >=0) {
        if (availableLanguages.indexOf("language-c") < 0) availableLanguages.push("language-c"); 
        if (availableLanguages.indexOf("language-cpp") < 0) availableLanguages.push("language-cpp"); 
      }
      // Set initial language
      var lang = window.localStorage.getItem("FMOD.Documents.selected-language");

      if (lang == null) {
        lang = "language-cpp";
      }

      setLanguage(lang);
  }
}

if (typeof module !== 'undefined') {
    module.exports = { initLanguageSelector: init };
} else {
  // Call our init function when the document is loaded. (https://plainjs.com/javascript/events/running-code-when-the-document-is-ready-15/)
  if (document.readyState != 'loading') init();
  else if (document.addEventListener) document.addEventListener('DOMContentLoaded', init);
  else document.attachEvent('onreadystatechange', function() { if (document.readyState == 'complete') init(); });
}
