let config = {
  // eslint-disable-next-line no-undef
  apiRoot:
    (process.env.NODE_ENV === 'development' ? 'http://127.0.0.1:2747' : window.API_ROOT) + '/api',
  timeout: 1500000,
  headers: {
    'Content-Type': 'application/json',
  },
}

export function setApiRoot(newApiRoot) {
  config.apiRoot = newApiRoot
  console.log(`API root changed to: ${newApiRoot}`)
}

export default config
