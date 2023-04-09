const form = document.querySelector('#upload-form');
const fileInput = document.querySelector('#file-input');
const progressBar = document.querySelector('#progress-bar');

form.addEventListener('submit', (e) => {
  e.preventDefault();

  const formData = new FormData();
  formData.append('file', fileInput.files[0]);

  const xhr = new XMLHttpRequest();
  xhr.open('POST', '/upload');

  xhr.upload.addEventListener('progress', (e) => {
    if (e.lengthComputable) {
      const percent = (e.loaded / e.total) * 100;
      progressBar.value = percent;
    }
  });

  xhr.addEventListener('load', (e) => {
    console.log('File uploaded successfully!');
  });

  xhr.addEventListener('error', (e) => {
    console.error('Error uploading file:', e);
  });

  xhr.send(formData);
});